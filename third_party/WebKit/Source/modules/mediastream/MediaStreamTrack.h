/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MediaStreamTrack_h
#define MediaStreamTrack_h

#include <memory>
#include "bindings/core/v8/ScriptPromise.h"
#include "core/dom/ContextLifecycleObserver.h"
#include "modules/EventTargetModules.h"
#include "modules/ModulesExport.h"
#include "platform/bindings/ActiveScriptWrappable.h"
#include "platform/mediastream/MediaStreamDescriptor.h"
#include "platform/mediastream/MediaStreamSource.h"
#include "platform/wtf/Forward.h"

namespace blink {

class AudioSourceProvider;
class ExceptionState;
class ImageCapture;
class MediaTrackCapabilities;
class MediaTrackConstraints;
class MediaStream;
class MediaTrackSettings;
class ScriptState;

class MODULES_EXPORT MediaStreamTrack
    : public EventTargetWithInlineData,
      public ActiveScriptWrappable<MediaStreamTrack>,
      public ContextLifecycleObserver,
      public MediaStreamSource::Observer {
  USING_GARBAGE_COLLECTED_MIXIN(MediaStreamTrack);
  DEFINE_WRAPPERTYPEINFO();

 public:
  static MediaStreamTrack* Create(ExecutionContext*, MediaStreamComponent*);
  ~MediaStreamTrack() override;

  String kind() const;
  String id() const;
  String label() const;

  bool enabled() const;
  void setEnabled(bool);

  bool muted() const;

  String ContentHint() const;
  void SetContentHint(const String&);

  String readyState() const;

  void stopTrack(ExceptionState&);
  virtual MediaStreamTrack* clone(ScriptState*);

  // This function is called when constrains have been successfully applied.
  // Called from UserMediaRequest when it succeeds. It is not IDL-exposed.
  void SetConstraints(const WebMediaConstraints&);

  void getCapabilities(MediaTrackCapabilities&);
  void getConstraints(MediaTrackConstraints&);
  void getSettings(MediaTrackSettings&);
  ScriptPromise applyConstraints(ScriptState*, const MediaTrackConstraints&);

  DEFINE_ATTRIBUTE_EVENT_LISTENER(mute);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(unmute);
  DEFINE_ATTRIBUTE_EVENT_LISTENER(ended);

  MediaStreamComponent* Component() { return component_; }
  bool Ended() const;

  void RegisterMediaStream(MediaStream*);
  void UnregisterMediaStream(MediaStream*);

  // EventTarget
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // ScriptWrappable
  bool HasPendingActivity() const final;

  // ContextLifecycleObserver
  void ContextDestroyed(ExecutionContext*) override;

  std::unique_ptr<AudioSourceProvider> CreateWebAudioSource();

  DECLARE_VIRTUAL_TRACE();

 private:
  friend class CanvasCaptureMediaStreamTrack;

  MediaStreamTrack(ExecutionContext*, MediaStreamComponent*);

  // MediaStreamSourceObserver
  void SourceChangedState() override;

  void PropagateTrackEnded();

  MediaStreamSource::ReadyState ready_state_;
  HeapHashSet<Member<MediaStream>> registered_media_streams_;
  bool is_iterating_registered_media_streams_;
  bool stopped_;
  Member<MediaStreamComponent> component_;
  Member<ImageCapture> image_capture_;
};

typedef HeapVector<Member<MediaStreamTrack>> MediaStreamTrackVector;

}  // namespace blink

#endif  // MediaStreamTrack_h
