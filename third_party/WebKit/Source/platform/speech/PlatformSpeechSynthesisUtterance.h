/*
 * Copyright (C) 2013 Apple Computer, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PlatformSpeechSynthesisUtterance_h
#define PlatformSpeechSynthesisUtterance_h

#include "platform/PlatformExport.h"
#include "platform/heap/Handle.h"
#include "platform/speech/PlatformSpeechSynthesisVoice.h"
#include "platform/wtf/MathExtras.h"
#include "platform/wtf/text/WTFString.h"

namespace blink {

class PlatformSpeechSynthesisUtteranceClient : public GarbageCollectedMixin {
 public:
  // Implement methods as needed.
 protected:
  virtual ~PlatformSpeechSynthesisUtteranceClient() {}
};

class PLATFORM_EXPORT PlatformSpeechSynthesisUtterance final
    : public GarbageCollectedFinalized<PlatformSpeechSynthesisUtterance> {
 public:
  static PlatformSpeechSynthesisUtterance* Create(
      PlatformSpeechSynthesisUtteranceClient*);

  const String& GetText() const { return text_; }
  void SetText(const String& text) { text_ = text; }

  const String& Lang() const { return lang_; }
  void SetLang(const String& lang) { lang_ = lang; }

  PlatformSpeechSynthesisVoice* Voice() const { return voice_.Get(); }
  void SetVoice(PlatformSpeechSynthesisVoice* voice) { voice_ = voice; }

  // Range = [0, 1] where 1 is the default.
  float Volume() const { return volume_; }
  void SetVolume(float volume) { volume_ = clampTo(volume, 0.0f, 1.0f); }

  // Range = [0.1, 10] where 1 is the default.
  float Rate() const { return rate_; }
  void SetRate(float rate) { rate_ = clampTo(rate, 0.1f, 10.0f); }

  // Range = [0, 2] where 1 is the default.
  float Pitch() const { return pitch_; }
  void SetPitch(float pitch) { pitch_ = clampTo(pitch, 0.0f, 2.0f); }

  double StartTime() const { return start_time_; }
  void SetStartTime(double start_time) { start_time_ = start_time; }

  PlatformSpeechSynthesisUtteranceClient* Client() const { return client_; }

  DECLARE_TRACE();

 private:
  explicit PlatformSpeechSynthesisUtterance(
      PlatformSpeechSynthesisUtteranceClient*);

  Member<PlatformSpeechSynthesisUtteranceClient> client_;
  String text_;
  String lang_;
  RefPtr<PlatformSpeechSynthesisVoice> voice_;
  float volume_;
  float rate_;
  float pitch_;
  double start_time_;
};

}  // namespace blink

#endif  // PlatformSpeechSynthesisUtterance_h
