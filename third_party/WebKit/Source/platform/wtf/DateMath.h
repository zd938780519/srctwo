/*
 * Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2010 Research In Motion Limited. All rights reserved.
 *
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Communicator client code, released
 * March 31, 1998.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either of the GNU General Public License Version 2 or later (the "GPL"),
 * or the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 */

#ifndef DateMath_h
#define DateMath_h

#include "platform/wtf/WTFExport.h"
#include "platform/wtf/text/WTFString.h"
#include <stdint.h>
#include <string.h>

namespace WTF {

WTF_EXPORT void InitializeDates();

// Not really math related, but this is currently the only shared place to put
// these.
WTF_EXPORT double ParseDateFromNullTerminatedCharacters(
    const char* date_string);
// dayOfWeek: [0, 6] 0 being Monday
// day: [1, 31]
// month: [0, 11]
// year: ex: 2011
// hours: [0, 23]
// minutes: [0, 59]
// seconds: [0, 59]
// utcOffset: [-720,720].
WTF_EXPORT String MakeRFC2822DateString(unsigned day_of_week,
                                        unsigned day,
                                        unsigned month,
                                        unsigned year,
                                        unsigned hours,
                                        unsigned minutes,
                                        unsigned seconds,
                                        int utc_offset);

const char kWeekdayName[7][4] = {"Mon", "Tue", "Wed", "Thu",
                                 "Fri", "Sat", "Sun"};
const char kMonthName[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char* const kMonthFullName[12] = {
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"};

const double kMinutesPerHour = 60.0;
const double kSecondsPerMinute = 60.0;
const double kMsPerSecond = 1000.0;
const double kMsPerMinute = 60.0 * 1000.0;
const double kMsPerHour = 60.0 * 60.0 * 1000.0;
const double kMsPerDay = 24.0 * 60.0 * 60.0 * 1000.0;

WTF_EXPORT bool IsLeapYear(int year);

// Returns the number of days from 1970-01-01 to the specified date.
WTF_EXPORT double DateToDaysFrom1970(int year, int month, int day);
WTF_EXPORT int MsToYear(double ms);
WTF_EXPORT int DayInYear(int year, int month, int day);
WTF_EXPORT int DayInYear(double ms, int year);
WTF_EXPORT int MonthFromDayInYear(int day_in_year, bool leap_year);
WTF_EXPORT int DayInMonthFromDayInYear(int day_in_year, bool leap_year);

// Returns milliseconds with UTC and DST.
WTF_EXPORT double ConvertToLocalTime(double ms);

}  // namespace WTF

using WTF::IsLeapYear;
using WTF::DateToDaysFrom1970;
using WTF::DayInMonthFromDayInYear;
using WTF::DayInYear;
using WTF::kMinutesPerHour;
using WTF::MonthFromDayInYear;
using WTF::kMsPerDay;
using WTF::kMsPerHour;
using WTF::kMsPerMinute;
using WTF::kMsPerSecond;
using WTF::MsToYear;
using WTF::kSecondsPerMinute;
using WTF::ParseDateFromNullTerminatedCharacters;
using WTF::MakeRFC2822DateString;
using WTF::ConvertToLocalTime;

#endif  // DateMath_h
