// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GeoNotifier_h
#define GeoNotifier_h

#include "modules/geolocation/PositionCallback.h"
#include "modules/geolocation/PositionErrorCallback.h"
#include "modules/geolocation/PositionOptions.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"

namespace blink {

class Geolocation;
class Geoposition;
class PositionError;

class GeoNotifier : public GarbageCollectedFinalized<GeoNotifier> {
 public:
  static GeoNotifier* Create(Geolocation* geolocation,
                             PositionCallback* position_callback,
                             PositionErrorCallback* position_error_callback,
                             const PositionOptions& options) {
    return new GeoNotifier(geolocation, position_callback,
                           position_error_callback, options);
  }
  DECLARE_TRACE();

  const PositionOptions& Options() const { return options_; }

  // Sets the given error as the fatal error if there isn't one yet.
  // Starts the timer with an interval of 0.
  void SetFatalError(PositionError*);

  bool UseCachedPosition() const { return use_cached_position_; }

  // Tells the notifier to use a cached position and starts its timer with
  // an interval of 0.
  void SetUseCachedPosition();

  void RunSuccessCallback(Geoposition*);
  void RunErrorCallback(PositionError*);

  void StartTimer();
  void StopTimer();

  // Runs the error callback if there is a fatal error. Otherwise, if a
  // cached position must be used, registers itself for receiving one.
  // Otherwise, the notifier has expired, and its error callback is run.
  void TimerFired(TimerBase*);

 private:
  GeoNotifier(Geolocation*,
              PositionCallback*,
              PositionErrorCallback*,
              const PositionOptions&);

  Member<Geolocation> geolocation_;
  Member<PositionCallback> success_callback_;
  Member<PositionErrorCallback> error_callback_;
  const PositionOptions options_;
  Timer<GeoNotifier> timer_;
  Member<PositionError> fatal_error_;
  bool use_cached_position_;
};

}  // namespace blink

#endif  // GeoNotifier_h
