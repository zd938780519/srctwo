// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_SHELL_BROWSER_SHELL_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_
#define EXTENSIONS_SHELL_BROWSER_SHELL_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_

#include "base/macros.h"
#include "content/public/browser/speech_recognition_event_listener.h"
#include "content/public/browser/speech_recognition_manager_delegate.h"

namespace extensions {
namespace speech {

class ShellSpeechRecognitionManagerDelegate
    : public content::SpeechRecognitionManagerDelegate,
      public content::SpeechRecognitionEventListener {
 public:
  ShellSpeechRecognitionManagerDelegate();
  ~ShellSpeechRecognitionManagerDelegate() override;

 private:
  // SpeechRecognitionEventListener methods.
  void OnRecognitionStart(int session_id) override;
  void OnAudioStart(int session_id) override;
  void OnEnvironmentEstimationComplete(int session_id) override;
  void OnSoundStart(int session_id) override;
  void OnSoundEnd(int session_id) override;
  void OnAudioEnd(int session_id) override;
  void OnRecognitionEnd(int session_id) override;
  void OnRecognitionResults(
      int session_id,
      const content::SpeechRecognitionResults& result) override;
  void OnRecognitionError(
      int session_id,
      const content::SpeechRecognitionError& error) override;
  void OnAudioLevelsChange(int session_id,
                           float volume,
                           float noise_volume) override;

  // SpeechRecognitionManagerDelegate methods.
  void CheckRecognitionIsAllowed(
      int session_id,
      base::OnceCallback<void(bool ask_user, bool is_allowed)> callback)
      override;
  content::SpeechRecognitionEventListener* GetEventListener() override;
  bool FilterProfanities(int render_process_id) override;

  static void CheckRenderViewType(
      base::OnceCallback<void(bool ask_user, bool is_allowed)> callback,
      int render_process_id,
      int render_view_id);

  DISALLOW_COPY_AND_ASSIGN(ShellSpeechRecognitionManagerDelegate);
};

}  // namespace speech
}  // namespace extensions

#endif  // EXTENSIONS_SHELL_BROWSER_SHELL_SPEECH_RECOGNITION_MANAGER_DELEGATE_H_
