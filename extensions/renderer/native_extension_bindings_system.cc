// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/renderer/native_extension_bindings_system.h"

#include "base/callback.h"
#include "base/command_line.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_macros.h"
#include "base/timer/elapsed_timer.h"
#include "content/public/child/worker_thread.h"
#include "content/public/common/console_message_level.h"
#include "content/public/common/content_switches.h"
#include "extensions/common/constants.h"
#include "extensions/common/event_filtering_info.h"
#include "extensions/common/extension_api.h"
#include "extensions/common/extension_messages.h"
#include "extensions/common/features/feature_provider.h"
#include "extensions/renderer/api_activity_logger.h"
#include "extensions/renderer/bindings/api_binding_bridge.h"
#include "extensions/renderer/bindings/api_binding_hooks.h"
#include "extensions/renderer/bindings/api_binding_js_util.h"
#include "extensions/renderer/chrome_setting.h"
#include "extensions/renderer/console.h"
#include "extensions/renderer/content_setting.h"
#include "extensions/renderer/declarative_content_hooks_delegate.h"
#include "extensions/renderer/easy_unlock_proximity_required_stub.h"
#include "extensions/renderer/extension_frame_helper.h"
#include "extensions/renderer/ipc_message_sender.h"
#include "extensions/renderer/module_system.h"
#include "extensions/renderer/script_context.h"
#include "extensions/renderer/script_context_set.h"
#include "extensions/renderer/storage_area.h"
#include "extensions/renderer/web_request_hooks.h"
#include "extensions/renderer/worker_thread_dispatcher.h"
#include "gin/converter.h"
#include "gin/handle.h"
#include "gin/per_context_data.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"

namespace extensions {

namespace {

const char kBindingsSystemPerContextKey[] = "extension_bindings_system";

// Returns true if the given |api| is a "prefixed" api of the |root_api|; that
// is, if the api begins with the root.
// For example, 'app.runtime' is a prefixed api of 'app'.
// This is designed to be used as a utility when iterating over a sorted map, so
// assumes that |api| is lexicographically greater than |root_api|.
bool IsPrefixedAPI(base::StringPiece api, base::StringPiece root_api) {
  DCHECK_NE(api, root_api);
  DCHECK_GT(api, root_api);
  return base::StartsWith(api, root_api, base::CompareCase::SENSITIVE) &&
         api[root_api.size()] == '.';
}

// Returns the first different level of the api specification between the given
// |api_name| and |reference|. For an api_name of 'app.runtime' and a reference
// of 'app', this returns 'app.runtime'. For an api_name of
// 'cast.streaming.session' and a reference of 'cast', this returns
// 'cast.streaming'. If reference is empty, this simply returns the first layer;
// so given 'app.runtime' and no reference, this returns 'app'.
base::StringPiece GetFirstDifferentAPIName(
    base::StringPiece api_name,
    base::StringPiece reference) {
  base::StringPiece::size_type dot =
      api_name.find('.', reference.empty() ? 0 : reference.size() + 1);
  if (dot == base::StringPiece::npos)
    return api_name;
  return api_name.substr(0, dot);
}

struct BindingsSystemPerContextData : public base::SupportsUserData::Data {
  BindingsSystemPerContextData(
      base::WeakPtr<NativeExtensionBindingsSystem> bindings_system)
      : bindings_system(bindings_system) {}
  ~BindingsSystemPerContextData() override {}

  v8::Global<v8::Object> api_object;
  v8::Global<v8::Object> internal_apis;
  base::WeakPtr<NativeExtensionBindingsSystem> bindings_system;
};

// If a 'chrome' property exists on the context's global and is an object,
// returns that.
// If a 'chrome' property exists but isn't an object, returns an empty Local.
// If no 'chrome' property exists (or is undefined), creates a new
// object, assigns it to Global().chrome, and returns it.
v8::Local<v8::Object> GetOrCreateChrome(v8::Local<v8::Context> context) {
  // Ensure that the creation context for any new chrome object is |context|.
  v8::Context::Scope context_scope(context);

  // TODO(devlin): This is a little silly. We expect that this may do the wrong
  // thing if the window has set some other 'chrome' (as in the case of script
  // doing 'window.chrome = true'), but we don't really handle it. It could also
  // throw exceptions or have unintended side effects.
  // On the one hand, anyone writing that code is probably asking for trouble.
  // On the other, it'd be nice to avoid. I wonder if we can?
  v8::Local<v8::String> chrome_string =
      gin::StringToSymbol(context->GetIsolate(), "chrome");
  v8::Local<v8::Value> chrome_value;
  if (!context->Global()->Get(context, chrome_string).ToLocal(&chrome_value))
    return v8::Local<v8::Object>();

  v8::Local<v8::Object> chrome_object;
  if (chrome_value->IsUndefined()) {
    chrome_object = v8::Object::New(context->GetIsolate());
    v8::Maybe<bool> success =
        context->Global()->CreateDataProperty(context, chrome_string,
                                              chrome_object);
    if (!success.IsJust() || !success.FromJust())
      return v8::Local<v8::Object>();
  } else if (chrome_value->IsObject()) {
    v8::Local<v8::Object> obj = chrome_value.As<v8::Object>();
    // The creation context of the `chrome` property could be different if a
    // different context (such as the parent of an about:blank iframe) assigned
    // it. Since in this case we know that the chrome object is not the one we
    // created, do not use it for bindings. This also avoids weirdness of having
    // bindings created in one context stored on a chrome object from another.
    // TODO(devlin): There might be a way of detecting if the browser created
    // the chrome object. For instance, we could add a v8::Private to the
    // chrome object we construct, and check if it's present. Unfortunately, we
    // need to a) track down each place we create the chrome object (it's not
    // just in extensions) and also see how much that would break.
    if (obj->CreationContext() == context)
      chrome_object = obj;
  }

  return chrome_object;
}

BindingsSystemPerContextData* GetBindingsDataFromContext(
    v8::Local<v8::Context> context) {
  gin::PerContextData* per_context_data = gin::PerContextData::From(context);
  if (!per_context_data)
    return nullptr;  // Context is shutting down.

  auto* data = static_cast<BindingsSystemPerContextData*>(
      per_context_data->GetUserData(kBindingsSystemPerContextKey));
  CHECK(data);
  if (!data->bindings_system) {
    NOTREACHED() << "Context outlived bindings system.";
    return nullptr;
  }

  return data;
}

// Returns the ScriptContext associated with the given v8::Context.
// TODO(devlin): Does this belong here, or should it be curried in as a
// callback? This is the only place we have knowledge of worker vs. non-worker
// threads here.
ScriptContext* GetScriptContext(v8::Local<v8::Context> context) {
  ScriptContext* script_context =
      content::WorkerThread::GetCurrentId() > 0
          ? WorkerThreadDispatcher::GetScriptContext()
          : ScriptContextSet::GetContextByV8Context(context);
  DCHECK(!script_context || script_context->v8_context() == context);
  return script_context;
}

// Same as above, but CHECKs the result.
ScriptContext* GetScriptContextChecked(v8::Local<v8::Context> context) {
  ScriptContext* script_context = GetScriptContext(context);
  CHECK(script_context);
  return script_context;
}

// Handler for calling safely into JS.
void CallJsFunction(v8::Local<v8::Function> function,
                    v8::Local<v8::Context> context,
                    int argc,
                    v8::Local<v8::Value> argv[]) {
  // TODO(devlin): Is using GetScriptContextChecked() (instead of
  // GetScriptContext()) safe? If our custom bindings can continue running after
  // contexts are removed from the set, it's likely not, but that's mostly a
  // bug. In a perfect world, we *should* be able to do this CHECK.
  ScriptContext* script_context = GetScriptContextChecked(context);
  script_context->SafeCallFunction(function, argc, argv);
}

v8::Global<v8::Value> CallJsFunctionSync(v8::Local<v8::Function> function,
                                         v8::Local<v8::Context> context,
                                         int argc,
                                         v8::Local<v8::Value> argv[]) {
  bool did_complete = false;
  v8::Global<v8::Value> result;
  auto callback = base::Bind([](
      v8::Isolate* isolate,
      bool* did_complete_out,
      v8::Global<v8::Value>* result_out,
      const std::vector<v8::Local<v8::Value>>& results) {
    *did_complete_out = true;
    // The locals are released after the callback is executed, so we need to
    // grab a persistent handle.
    if (!results.empty() && !results[0].IsEmpty())
      result_out->Reset(isolate, results[0]);
  }, base::Unretained(context->GetIsolate()),
     base::Unretained(&did_complete), base::Unretained(&result));

  ScriptContext* script_context = GetScriptContextChecked(context);
  script_context->SafeCallFunction(function, argc, argv, callback);
  CHECK(did_complete) << "expected script to execute synchronously";
  return result;
}

void AddConsoleError(v8::Local<v8::Context> context, const std::string& error) {
  ScriptContext* script_context = GetScriptContext(context);
  // Note: |script_context| may be null. During context tear down, we remove the
  // script context from the ScriptContextSet, so it's not findable by
  // GetScriptContext. In theory, we shouldn't be running any bindings code
  // after this point, but it seems that we are in at least some places.
  // TODO(devlin): Investigate. At least one place this manifests is in
  // messaging binding tear down exhibited by
  // MessagingApiTest.MessagingBackgroundOnly.
  // console::AddMessage() can handle a null script context.
  console::AddMessage(script_context, content::CONSOLE_MESSAGE_LEVEL_ERROR,
                      error);
}

// Returns the API schema indicated by |api_name|.
const base::DictionaryValue& GetAPISchema(const std::string& api_name) {
  const base::DictionaryValue* schema =
      ExtensionAPI::GetSharedInstance()->GetSchema(api_name);
  CHECK(schema) << api_name;
  return *schema;
}

// Returns true if the feature specified by |name| is available to the given
// |context|.
bool IsAPIFeatureAvailable(v8::Local<v8::Context> context,
                           const std::string& name) {
  ScriptContext* script_context = GetScriptContextChecked(context);
  return script_context->GetAvailability(name).is_available();
}

// Instantiates the binding object for the given |name|. |name| must specify a
// specific feature.
v8::Local<v8::Object> CreateRootBinding(v8::Local<v8::Context> context,
                                        ScriptContext* script_context,
                                        const std::string& name,
                                        APIBindingsSystem* bindings_system) {
  APIBindingHooks* hooks = nullptr;
  v8::Local<v8::Object> binding_object =
      bindings_system->CreateAPIInstance(name, context, &hooks);

  gin::Handle<APIBindingBridge> bridge_handle = gin::CreateHandle(
      context->GetIsolate(),
      new APIBindingBridge(hooks, context, binding_object,
                           script_context->GetExtensionID(),
                           script_context->GetContextTypeDescription(),
                           base::Bind(&CallJsFunction)));
  v8::Local<v8::Value> native_api_bridge = bridge_handle.ToV8();
  script_context->module_system()->OnNativeBindingCreated(name,
                                                          native_api_bridge);

  return binding_object;
}

// Creates the binding object for the given |root_name|. This can be
// complicated, since APIs may have prefixed names, like 'app.runtime' or
// 'system.cpu'. This method accepts the first name (i.e., the key that we are
// looking for on the chrome object, such as 'app') and returns the fully
// instantiated binding, including prefixed APIs. That is, given 'app', this
// will instantiate 'app', 'app.runtime', and 'app.window'.
//
// NOTE(devlin): We could do the prefixed apis lazily; however, it's not clear
// how much of a win it would be. It's less overhead here than in the general
// case (instantiating a handful of APIs instead of all of them), and it's more
// likely they will be used (since the extension is already accessing the
// parent).
// TODO(devlin): We should be creating ObjectTemplates for these so that we only
// do this work once. APIBindings (for the single API) already do this.
v8::Local<v8::Object> CreateFullBinding(
    v8::Local<v8::Context> context,
    ScriptContext* script_context,
    APIBindingsSystem* bindings_system,
    const FeatureProvider* api_feature_provider,
    const std::string& root_name) {
  const FeatureMap& features = api_feature_provider->GetAllFeatures();
  auto lower = features.lower_bound(root_name);
  DCHECK(lower != features.end());

  // Some bindings have a prefixed name, like app.runtime, where 'app' and
  // 'app.runtime' are, in fact, separate APIs. It's also possible for a
  // context to have access to 'app.runtime', but not to 'app'. For this, we
  // either instantiate the 'app' binding fully (if the context has access), or
  // else use an empty object (so we can still instantiate 'app.runtime').
  v8::Local<v8::Object> root_binding;
  if (lower->first == root_name) {
    const Feature* feature = lower->second.get();
    if (script_context->IsAnyFeatureAvailableToContext(
            *feature, CheckAliasStatus::NOT_ALLOWED)) {
      // If this feature is an alias for a different API, use the other binding
      // as the basis for the API contents.
      const std::string& source_name =
          feature->source().empty() ? root_name : feature->source();
      root_binding = CreateRootBinding(context, script_context, source_name,
                                       bindings_system);
    }
    ++lower;
  }

  // Look for any bindings that would be on the same object. Any of these would
  // start with the same base name (e.g. 'app') + '.' (since '.' is < x for any
  // isalpha(x)).
  std::string upper = root_name + static_cast<char>('.' + 1);
  base::StringPiece last_binding_name;
  // The following loop is a little painful because we have crazy binding names
  // and syntaxes. The way this works is as follows:
  // Look at each feature after the root feature we passed in. If there exists
  // a (non-child) feature with a prefixed name, create the full binding for
  // the object that the next feature is on. Then, iterate past any features
  // already instantiated by that, and continue until there are no more features
  // prefixed by the root API.
  // As a concrete example, we can look at the cast APIs (cast and
  // cast.streaming.*)
  // Start with vanilla 'cast', and instantiate that.
  // Then iterate over features, and see 'cast.streaming.receiverSession'.
  // 'cast.streaming.receiverSession' is a prefixed API of 'cast', but we find
  // the first level of difference, which is 'cast.streaming', and instantiate
  // that object completely (through recursion).
  // The next feature is 'cast.streaming.rtpStream', but this is a prefixed API
  // of 'cast.streaming', which we just instantiated completely (including
  // 'cast.streaming.rtpStream'), so we continue.
  // Iterate until all cast.* features are created.
  // TODO(devlin): This is bonkers, but what's the better way? We could extract
  // this out to be a more readable Visitor implementation, but is it worth it
  // for this one place? Ideally, we'd have a less convoluted feature
  // representation (some kind of tree would make this trivial), but for now, we
  // have strings.
  // On the upside, most APIs are not prefixed at all, and this loop is never
  // entered.
  for (auto iter = lower; iter != features.end() && iter->first < upper;
       ++iter) {
    if (iter->second->IsInternal())
      continue;

    if (IsPrefixedAPI(iter->first, last_binding_name)) {
      // Instantiating |last_binding_name| must have already instantiated
      // iter->first.
      continue;
    }

    // If this API has a parent feature (and isn't marked 'noparent'),
    // then this must be a function or event, so we should not register.
    if (api_feature_provider->GetParent(*iter->second) != nullptr)
      continue;

    base::StringPiece binding_name =
        GetFirstDifferentAPIName(iter->first, root_name);

    v8::Local<v8::Object> nested_binding =
        CreateFullBinding(context, script_context, bindings_system,
                          api_feature_provider, binding_name.as_string());
    // It's possible that we don't create a binding if no features or
    // prefixed features are available to the context.
    if (nested_binding.IsEmpty())
      continue;

    if (root_binding.IsEmpty())
      root_binding = v8::Object::New(context->GetIsolate());

    // The nested api name contains a '.', e.g. 'app.runtime', but we want to
    // expose it on the object simply as 'runtime'.
    // Cache the last_binding_name now before mangling it.
    last_binding_name = binding_name;
    DCHECK_NE(base::StringPiece::npos, binding_name.rfind('.'));
    base::StringPiece accessor_name =
        binding_name.substr(binding_name.rfind('.') + 1);
    v8::Local<v8::String> nested_name =
        gin::StringToSymbol(context->GetIsolate(), accessor_name);
    v8::Maybe<bool> success =
        root_binding->CreateDataProperty(context, nested_name, nested_binding);
    if (!success.IsJust() || !success.FromJust())
      return v8::Local<v8::Object>();
  }

  return root_binding;
}

}  // namespace

NativeExtensionBindingsSystem::NativeExtensionBindingsSystem(
    std::unique_ptr<IPCMessageSender> ipc_message_sender)
    : ipc_message_sender_(std::move(ipc_message_sender)),
      api_system_(
          base::Bind(&CallJsFunction),
          base::Bind(&CallJsFunctionSync),
          base::Bind(&GetAPISchema),
          base::Bind(&IsAPIFeatureAvailable),
          base::Bind(&NativeExtensionBindingsSystem::SendRequest,
                     base::Unretained(this)),
          base::Bind(&NativeExtensionBindingsSystem::OnEventListenerChanged,
                     base::Unretained(this)),
          base::Bind(&APIActivityLogger::LogAPICall),
          base::Bind(&AddConsoleError),
          APILastError(base::Bind(&GetLastErrorParents),
                       base::Bind(&AddConsoleError))),
      weak_factory_(this) {
  api_system_.RegisterCustomType("storage.StorageArea",
                                 base::Bind(&StorageArea::CreateStorageArea));
  api_system_.RegisterCustomType(
      "preferencesPrivate.EasyUnlockProximityRequired",
      base::Bind(&EasyUnlockProximityRequiredStub::Create));
  api_system_.RegisterCustomType("types.ChromeSetting",
                                 base::Bind(&ChromeSetting::Create));
  api_system_.RegisterCustomType(
      "contentSettings.ContentSetting",
      base::Bind(&ContentSetting::Create, base::Bind(&CallJsFunction)));
  api_system_.GetHooksForAPI("webRequest")
      ->SetDelegate(std::make_unique<WebRequestHooks>());
  api_system_.GetHooksForAPI("declarativeContent")
      ->SetDelegate(std::make_unique<DeclarativeContentHooksDelegate>());
}

NativeExtensionBindingsSystem::~NativeExtensionBindingsSystem() {}

void NativeExtensionBindingsSystem::DidCreateScriptContext(
    ScriptContext* context) {
  v8::Isolate* isolate = context->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> v8_context = context->v8_context();
  gin::PerContextData* per_context_data = gin::PerContextData::From(v8_context);
  DCHECK(per_context_data);
  DCHECK(!per_context_data->GetUserData(kBindingsSystemPerContextKey));
  auto data = std::make_unique<BindingsSystemPerContextData>(
      weak_factory_.GetWeakPtr());
  per_context_data->SetUserData(kBindingsSystemPerContextKey, std::move(data));

  if (get_internal_api_.IsEmpty()) {
    get_internal_api_.Set(
        isolate, v8::FunctionTemplate::New(
                     isolate, &NativeExtensionBindingsSystem::GetInternalAPI,
                     v8::Local<v8::Value>(), v8::Local<v8::Signature>(), 0,
                     v8::ConstructorBehavior::kThrow));
  }

  // Note: it's a shame we can't delay this (until, say, we knew an API would
  // actually be used), but it's needed for some of our crazier hooks, like
  // web/guest view.
  context->module_system()->SetGetInternalAPIHook(
      get_internal_api_.Get(isolate));
  context->module_system()->SetJSBindingUtilGetter(
      base::Bind(&NativeExtensionBindingsSystem::GetJSBindingUtil,
                 weak_factory_.GetWeakPtr()));
}

void NativeExtensionBindingsSystem::WillReleaseScriptContext(
    ScriptContext* context) {
  v8::HandleScope handle_scope(context->isolate());
  api_system_.WillReleaseContext(context->v8_context());
}

void NativeExtensionBindingsSystem::UpdateBindingsForContext(
    ScriptContext* context) {
  base::ElapsedTimer timer;
  v8::Isolate* isolate = context->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> v8_context = context->v8_context();
  v8::Local<v8::Object> chrome = GetOrCreateChrome(v8_context);
  if (chrome.IsEmpty())
    return;

  DCHECK(GetBindingsDataFromContext(v8_context));

  auto set_accessor = [chrome, isolate,
                       v8_context](base::StringPiece accessor_name) {
    v8::Local<v8::String> api_name =
        gin::StringToSymbol(isolate, accessor_name);
    v8::Maybe<bool> success = chrome->SetAccessor(
        v8_context, api_name, &BindingAccessor, nullptr, api_name);
    return success.IsJust() && success.FromJust();
  };

  bool is_webpage = false;
  switch (context->context_type()) {
    case Feature::UNSPECIFIED_CONTEXT:
    case Feature::WEB_PAGE_CONTEXT:
    case Feature::BLESSED_WEB_PAGE_CONTEXT:
      is_webpage = true;
      break;
    case Feature::SERVICE_WORKER_CONTEXT:
      DCHECK(ExtensionsClient::Get()
                 ->ExtensionAPIEnabledInExtensionServiceWorkers());
    // Intentional fallthrough.
    case Feature::BLESSED_EXTENSION_CONTEXT:
    case Feature::LOCK_SCREEN_EXTENSION_CONTEXT:
    case Feature::UNBLESSED_EXTENSION_CONTEXT:
    case Feature::CONTENT_SCRIPT_CONTEXT:
    case Feature::WEBUI_CONTEXT:
      is_webpage = false;
  }

  if (is_webpage) {
    // Hard-code registration of any APIs that are exposed to webpage-like
    // contexts, because it's more expensive to iterate over all the existing
    // features when only a handful could ever be available.
    // All of the same permission checks will still apply.
    // TODO(devlin): It could be interesting to apply this same logic to all
    // context types, especially on a given platform. Something to think about
    // for when we generate features.
    for (const char* feature_name : kWebAvailableFeatures) {
      if (context->GetAvailability(feature_name).is_available() &&
          !set_accessor(feature_name)) {
        LOG(ERROR) << "Failed to create API on Chrome object.";
        return;
      }
    }

    // Runtime is special (see IsRuntimeAvailableToContext()).
    if (IsRuntimeAvailableToContext(context) && !set_accessor("runtime"))
      LOG(ERROR) << "Failed to create API on Chrome object.";

    LogUpdateBindingsForContextTime(context->context_type(), timer.Elapsed());
    return;
  }

  FeatureCache::FeatureNameVector features =
      feature_cache_.GetAvailableFeatures(context->context_type(),
                                          context->extension(), context->url());
  base::StringPiece last_accessor;
  for (const std::string& feature : features) {
    // If we've already set up an accessor for the immediate property of the
    // chrome object, we don't need to do more.
    if (IsPrefixedAPI(feature, last_accessor))
      continue;

    // We've found an API that's available to the extension. Normally, we will
    // expose this under the name of the feature (e.g., 'tabs'), but in some
    // cases, this will be a prefixed API, such as 'app.runtime'. Find what the
    // property on the chrome object is named, and use that. So in the case of
    // 'app.runtime', we surface a getter for simply 'app'.
    //
    // TODO(devlin): UpdateBindingsForContext can be called during context
    // creation, but also when e.g. permissions change. Do we need to be
    // checking for whether or not the API already exists on the object as well
    // as if we need to remove any existing APIs?
    base::StringPiece accessor_name =
        GetFirstDifferentAPIName(feature, base::StringPiece());
    last_accessor = accessor_name;
    if (!set_accessor(accessor_name)) {
      LOG(ERROR) << "Failed to create API on Chrome object.";
      return;
    }
  }

  LogUpdateBindingsForContextTime(context->context_type(), timer.Elapsed());
}

void NativeExtensionBindingsSystem::DispatchEventInContext(
    const std::string& event_name,
    const base::ListValue* event_args,
    const EventFilteringInfo* filtering_info,
    ScriptContext* context) {
  v8::HandleScope handle_scope(context->isolate());
  v8::Context::Scope context_scope(context->v8_context());
  api_system_.FireEventInContext(event_name, context->v8_context(), *event_args,
                                 filtering_info);
}

bool NativeExtensionBindingsSystem::HasEventListenerInContext(
    const std::string& event_name,
    ScriptContext* context) {
  v8::HandleScope handle_scope(context->isolate());
  return api_system_.event_handler()->HasListenerForEvent(
      event_name, context->v8_context());
}

void NativeExtensionBindingsSystem::HandleResponse(
    int request_id,
    bool success,
    const base::ListValue& response,
    const std::string& error) {
  // Some API calls result in failure, but don't set an error. Use a generic and
  // unhelpful error string.
  // TODO(devlin): Track these down and fix them. See crbug.com/648275.
  api_system_.CompleteRequest(
      request_id, response,
      !success && error.empty() ? "Unknown error." : error);
  ipc_message_sender_->SendOnRequestResponseReceivedIPC(request_id);
}

RequestSender* NativeExtensionBindingsSystem::GetRequestSender() {
  return nullptr;
}

IPCMessageSender* NativeExtensionBindingsSystem::GetIPCMessageSender() {
  return ipc_message_sender_.get();
}

void NativeExtensionBindingsSystem::OnExtensionPermissionsUpdated(
    const ExtensionId& id) {
  feature_cache_.InvalidateExtension(id);
}

void NativeExtensionBindingsSystem::OnExtensionRemoved(const ExtensionId& id) {
  feature_cache_.InvalidateExtension(id);
}

v8::Local<v8::Object> NativeExtensionBindingsSystem::GetAPIObjectForTesting(
    ScriptContext* context,
    const std::string& api_name) {
  return GetAPIHelper(context->v8_context(),
                      gin::StringToSymbol(context->isolate(), api_name));
}

void NativeExtensionBindingsSystem::BindingAccessor(
    v8::Local<v8::Name> name,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = info.Holder()->CreationContext();

  // We use info.Data() to store a real name here instead of using the provided
  // one to handle any weirdness from the caller (non-existent strings, etc).
  v8::Local<v8::String> api_name = info.Data().As<v8::String>();
  v8::Local<v8::Object> binding = GetAPIHelper(context, api_name);
  if (!binding.IsEmpty())
    info.GetReturnValue().Set(binding);
}

// static
v8::Local<v8::Object> NativeExtensionBindingsSystem::GetAPIHelper(
    v8::Local<v8::Context> context,
    v8::Local<v8::String> api_name) {
  BindingsSystemPerContextData* data = GetBindingsDataFromContext(context);
  if (!data)
    return v8::Local<v8::Object>();

  v8::Isolate* isolate = context->GetIsolate();
  v8::Local<v8::Object> apis;
  if (data->api_object.IsEmpty()) {
    apis = v8::Object::New(isolate);
    data->api_object = v8::Global<v8::Object>(isolate, apis);
  } else {
    apis = data->api_object.Get(isolate);
  }

  v8::Maybe<bool> has_property = apis->HasRealNamedProperty(context, api_name);
  if (!has_property.IsJust())
    return v8::Local<v8::Object>();

  if (has_property.FromJust()) {
    v8::Local<v8::Value> value =
        apis->GetRealNamedProperty(context, api_name).ToLocalChecked();
    DCHECK(value->IsObject());
    return value.As<v8::Object>();
  }

  ScriptContext* script_context = GetScriptContextChecked(context);
  std::string api_name_string;
  CHECK(
      gin::Converter<std::string>::FromV8(isolate, api_name, &api_name_string));

  base::ElapsedTimer timer;
  v8::Local<v8::Object> root_binding = CreateFullBinding(
      context, script_context, &data->bindings_system->api_system_,
      FeatureProvider::GetAPIFeatures(), api_name_string);
  if (root_binding.IsEmpty())
    return v8::Local<v8::Object>();

  v8::Maybe<bool> success =
      apis->CreateDataProperty(context, api_name, root_binding);
  if (!success.IsJust() || !success.FromJust())
    return v8::Local<v8::Object>();

  UMA_HISTOGRAM_CUSTOM_COUNTS("Extensions.Bindings.NativeBindingCreationTime",
                              timer.Elapsed().InMicroseconds(), 1, 10000000,
                              50);
  return root_binding;
}

v8::Local<v8::Object> NativeExtensionBindingsSystem::GetLastErrorParents(
    v8::Local<v8::Context> context,
    v8::Local<v8::Object>* secondary_parent) {
  if (secondary_parent &&
      IsAPIFeatureAvailable(context, "extension.lastError")) {
    *secondary_parent = GetAPIHelper(
        context, gin::StringToSymbol(context->GetIsolate(), "extension"));
  }

  return GetAPIHelper(context,
                      gin::StringToSymbol(context->GetIsolate(), "runtime"));
}

// static
void NativeExtensionBindingsSystem::GetInternalAPI(
    const v8::FunctionCallbackInfo<v8::Value>& info) {
  CHECK_EQ(1, info.Length());
  CHECK(info[0]->IsString());

  v8::Isolate* isolate = info.GetIsolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = isolate->GetCurrentContext();

  v8::Local<v8::String> v8_name = info[0].As<v8::String>();

  BindingsSystemPerContextData* data = GetBindingsDataFromContext(context);
  CHECK(data);

  v8::Local<v8::Object> internal_apis;
  if (data->internal_apis.IsEmpty()) {
    internal_apis = v8::Object::New(isolate);
    data->internal_apis.Reset(isolate, internal_apis);
  } else {
    internal_apis = data->internal_apis.Get(isolate);
  }

  v8::Maybe<bool> has_property =
      internal_apis->HasOwnProperty(context, v8_name);
  if (!has_property.IsJust())
    return;

  if (has_property.FromJust()) {
    // API was already instantiated.
    info.GetReturnValue().Set(
        internal_apis->GetRealNamedProperty(context, v8_name).ToLocalChecked());
    return;
  }

  std::string api_name = gin::V8ToString(info[0]);
  const Feature* feature = FeatureProvider::GetAPIFeature(api_name);
  ScriptContext* script_context = GetScriptContextChecked(context);
  if (!feature ||
      !script_context->IsAnyFeatureAvailableToContext(
          *feature, CheckAliasStatus::NOT_ALLOWED)) {
    NOTREACHED();
    return;
  }

  CHECK(feature->IsInternal());

  // We don't need to go through CreateFullBinding here because internal APIs
  // are always acquired through getInternalBinding and specified by full name,
  // rather than through access on the chrome object. So we can just instantiate
  // a binding keyed with any name, even a prefixed one (e.g.
  // 'app.currentWindowInternal').
  v8::Local<v8::Object> api_binding = CreateRootBinding(
      context, script_context, api_name, &data->bindings_system->api_system_);

  if (api_binding.IsEmpty())
    return;

  v8::Maybe<bool> success =
      internal_apis->CreateDataProperty(context, v8_name, api_binding);
  if (!success.IsJust() || !success.FromJust())
    return;

  info.GetReturnValue().Set(api_binding);
}

void NativeExtensionBindingsSystem::SendRequest(
    std::unique_ptr<APIRequestHandler::Request> request,
    v8::Local<v8::Context> context) {
  ScriptContext* script_context = GetScriptContextChecked(context);

  GURL url;
  blink::WebLocalFrame* frame = script_context->web_frame();
  if (frame && !frame->GetDocument().IsNull())
    url = frame->GetDocument().Url();
  else
    url = script_context->url();

  auto params = std::make_unique<ExtensionHostMsg_Request_Params>();
  params->name = request->method_name;
  params->arguments.Swap(request->arguments.get());
  params->extension_id = script_context->GetExtensionID();
  params->source_url = url;
  params->request_id = request->request_id;
  params->has_callback = request->has_callback;
  params->user_gesture = request->has_user_gesture;
  // The IPC sender will update these members, if appropriate.
  params->worker_thread_id = -1;
  params->service_worker_version_id = kInvalidServiceWorkerVersionId;

  ipc_message_sender_->SendRequestIPC(script_context, std::move(params),
                                      request->thread);
}

void NativeExtensionBindingsSystem::OnEventListenerChanged(
    const std::string& event_name,
    binding::EventListenersChanged change,
    const base::DictionaryValue* filter,
    bool update_lazy_listeners,
    v8::Local<v8::Context> context) {
  ScriptContext* script_context = GetScriptContextChecked(context);
  // Note: Check context_type() first to avoid accessing ExtensionFrameHelper on
  // a worker thread.
  bool is_lazy =
      update_lazy_listeners &&
      (script_context->context_type() == Feature::SERVICE_WORKER_CONTEXT ||
       ExtensionFrameHelper::IsContextForEventPage(script_context));
  // We only remove a lazy listener if the listener removal was triggered
  // manually by the extension.

  if (filter) {  // Filtered event listeners.
    DCHECK(filter);
    if (change == binding::EventListenersChanged::HAS_LISTENERS) {
      ipc_message_sender_->SendAddFilteredEventListenerIPC(
          script_context, event_name, *filter, is_lazy);
    } else {
      DCHECK_EQ(binding::EventListenersChanged::NO_LISTENERS, change);
      ipc_message_sender_->SendRemoveFilteredEventListenerIPC(
          script_context, event_name, *filter, is_lazy);
    }
  } else {  // Unfiltered event listeners.
    if (change == binding::EventListenersChanged::HAS_LISTENERS) {
      // TODO(devlin): The JS bindings code only adds one listener per extension
      // per event per process, whereas this is one listener per context per
      // event per process. Typically, this won't make a difference, but it
      // could if there are multiple contexts for the same extension (e.g.,
      // multiple frames). In that case, it would result in extra IPCs being
      // sent. I'm not sure it's a big enough deal to warrant refactoring.
      ipc_message_sender_->SendAddUnfilteredEventListenerIPC(script_context,
                                                             event_name);
      if (is_lazy) {
        ipc_message_sender_->SendAddUnfilteredLazyEventListenerIPC(
            script_context, event_name);
      }
    } else {
      DCHECK_EQ(binding::EventListenersChanged::NO_LISTENERS, change);
      ipc_message_sender_->SendRemoveUnfilteredEventListenerIPC(script_context,
                                                                event_name);
      if (is_lazy) {
        ipc_message_sender_->SendRemoveUnfilteredLazyEventListenerIPC(
            script_context, event_name);
      }
    }
  }
}

void NativeExtensionBindingsSystem::GetJSBindingUtil(
    v8::Local<v8::Context> context,
    v8::Local<v8::Value>* binding_util_out) {
  gin::Handle<APIBindingJSUtil> handle = gin::CreateHandle(
      context->GetIsolate(),
      new APIBindingJSUtil(
          api_system_.type_reference_map(), api_system_.request_handler(),
          api_system_.event_handler(), api_system_.exception_handler(),
          base::Bind(&CallJsFunction)));
  *binding_util_out = handle.ToV8();
}

}  // namespace extensions