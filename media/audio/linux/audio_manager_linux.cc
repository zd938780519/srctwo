// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/command_line.h"
#include "base/memory/ptr_util.h"
#include "base/metrics/histogram_macros.h"
#include "media/base/media_switches.h"

#if defined(USE_ALSA)
#include "media/audio/alsa/audio_manager_alsa.h"
#else
#include "media/audio/fake_audio_manager.h"
#endif
#if defined(USE_CRAS)
#include "media/audio/cras/audio_manager_cras.h"
#endif
#if defined(USE_PULSEAUDIO)
#include "media/audio/pulse/audio_manager_pulse.h"
#include "media/audio/pulse/pulse_util.h"
#endif

namespace media {

enum LinuxAudioIO {
  kPulse,
  kAlsa,
  kCras,
  kAudioIOMax = kCras  // Must always be equal to largest logged entry.
};

std::unique_ptr<media::AudioManager> CreateAudioManager(
    std::unique_ptr<AudioThread> audio_thread,
    AudioLogFactory* audio_log_factory) {
#if defined(USE_CRAS)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kUseCras)) {
    UMA_HISTOGRAM_ENUMERATION("Media.LinuxAudioIO", kCras, kAudioIOMax + 1);
    return base::MakeUnique<AudioManagerCras>(std::move(audio_thread),
                                              audio_log_factory);
  }
#endif

#if defined(USE_PULSEAUDIO)
  pa_threaded_mainloop* pa_mainloop = nullptr;
  pa_context* pa_context = nullptr;
  if (pulse::InitPulse(&pa_mainloop, &pa_context)) {
    UMA_HISTOGRAM_ENUMERATION("Media.LinuxAudioIO", kPulse, kAudioIOMax + 1);
    return base::MakeUnique<AudioManagerPulse>(
        std::move(audio_thread), audio_log_factory, pa_mainloop, pa_context);
  }
  DVLOG(1) << "PulseAudio is not available on the OS";
#endif

#if defined(USE_ALSA)
  UMA_HISTOGRAM_ENUMERATION("Media.LinuxAudioIO", kAlsa, kAudioIOMax + 1);
  return base::MakeUnique<AudioManagerAlsa>(std::move(audio_thread),
                                            audio_log_factory);
#else
  return base::MakeUnique<FakeAudioManager>(std::move(audio_thread),
                                            audio_log_factory);
#endif
}

}  // namespace media
