// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/renderer/prerender/prerender_helper.h"

#include "base/metrics/field_trial.h"
#include "base/metrics/histogram.h"
#include "chrome/common/prerender_messages.h"
#include "content/public/renderer/document_state.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_view.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebView.h"

using content::DocumentState;

namespace {

// Updates the visibility state of the RenderFrame.  Must be called whenever
// prerendering starts or finishes and the page is about to be show.  At both
// those times, the RenderFrame is hidden.
void UpdateVisibilityState(content::RenderFrame* render_frame) {
  // TODO(jam): until the prerendering code works on frames instead of views, we
  // have to do this awkward check.
  content::RenderView* render_view = render_frame->GetRenderView();
  if (render_view->GetMainRenderFrame() == render_frame) {
    render_view->GetWebView()->SetVisibilityState(
        render_frame->GetVisibilityState(), false);
  }
}

}  // namespace

namespace prerender {

PrerenderHelper::PrerenderHelper(content::RenderFrame* render_frame,
                                 PrerenderMode prerender_mode)
    : content::RenderFrameObserver(render_frame),
      content::RenderFrameObserverTracker<PrerenderHelper>(render_frame),
      prerender_mode_(prerender_mode) {
  DCHECK_NE(prerender_mode_, NO_PRERENDER);
  UpdateVisibilityState(render_frame);
}

PrerenderHelper::PrerenderHelper(content::RenderFrame* sub_frame)
    : PrerenderHelper(sub_frame,
                      GetPrerenderMode(
                          sub_frame->GetRenderView()->GetMainRenderFrame())) {
  DCHECK(!render_frame()->IsMainFrame());
}

PrerenderHelper::~PrerenderHelper() {
}

// static.
bool PrerenderHelper::IsPrerendering(const content::RenderFrame* render_frame) {
  return PrerenderHelper::GetPrerenderMode(render_frame) != NO_PRERENDER;
}

// static.
PrerenderMode PrerenderHelper::GetPrerenderMode(
    const content::RenderFrame* render_frame) {
  PrerenderHelper* helper = PrerenderHelper::Get(render_frame);
  if (!helper)
    return NO_PRERENDER;

  DCHECK_NE(helper->prerender_mode_, NO_PRERENDER);
  return helper->prerender_mode_;
}

bool PrerenderHelper::OnMessageReceived(
    const IPC::Message& message) {
  IPC_BEGIN_MESSAGE_MAP(PrerenderHelper, message)
    IPC_MESSAGE_HANDLER(PrerenderMsg_SetIsPrerendering, OnSetIsPrerendering)
  IPC_END_MESSAGE_MAP()
  // Return false on PrerenderMsg_SetIsPrerendering so other observers can see
  // the message.
  return false;
}

void PrerenderHelper::OnSetIsPrerendering(PrerenderMode mode) {
  // Immediately after construction, |this| may receive the message that
  // triggered its creation.  If so, ignore it.
  if (mode != prerender::NO_PRERENDER)
    return;

  content::RenderFrame* frame = render_frame();
  // |this| must be deleted so PrerenderHelper::IsPrerendering returns false
  // when the visibility state is updated, so the visibility state string will
  // not be "prerendered".
  delete this;

  UpdateVisibilityState(frame);
}

void PrerenderHelper::OnDestruct() {
  delete this;
}

}  // namespace prerender
