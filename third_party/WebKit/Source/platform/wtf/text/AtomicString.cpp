/*
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2013 Apple Inc. All rights
 * reserved.
 * Copyright (C) 2010 Patrick Gansterer <paroga@paroga.com>
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include "platform/wtf/text/AtomicString.h"

#include "platform/wtf/dtoa.h"
#include "platform/wtf/text/AtomicStringTable.h"
#include "platform/wtf/text/IntegerToStringConversion.h"
#include "platform/wtf/text/StringImpl.h"

namespace WTF {

static_assert(sizeof(AtomicString) == sizeof(String),
              "AtomicString and String must be same size");

AtomicString::AtomicString(const LChar* chars, unsigned length)
    : string_(AtomicStringTable::Instance().Add(chars, length)) {}

AtomicString::AtomicString(const UChar* chars, unsigned length)
    : string_(AtomicStringTable::Instance().Add(chars, length)) {}

AtomicString::AtomicString(const UChar* chars)
    : string_(AtomicStringTable::Instance().Add(
          chars,
          chars ? LengthOfNullTerminatedString(chars) : 0)) {}

RefPtr<StringImpl> AtomicString::AddSlowCase(StringImpl* string) {
  DCHECK(!string->IsAtomic());
  return AtomicStringTable::Instance().Add(string);
}

AtomicString AtomicString::FromUTF8(const char* chars, size_t length) {
  if (!chars)
    return g_null_atom;
  if (!length)
    return g_empty_atom;
  return AtomicString(
      AtomicStringTable::Instance().AddUTF8(chars, chars + length));
}

AtomicString AtomicString::FromUTF8(const char* chars) {
  if (!chars)
    return g_null_atom;
  if (!*chars)
    return g_empty_atom;
  return AtomicString(AtomicStringTable::Instance().AddUTF8(chars, nullptr));
}

AtomicString AtomicString::DeprecatedLower() const {
  // Note: This is a hot function in the Dromaeo benchmark.
  StringImpl* impl = this->Impl();
  if (UNLIKELY(!impl))
    return *this;
  RefPtr<StringImpl> new_impl = impl->LowerUnicode();
  if (LIKELY(new_impl == impl))
    return *this;
  return AtomicString(String(std::move(new_impl)));
}

AtomicString AtomicString::LowerASCII() const {
  StringImpl* impl = this->Impl();
  if (UNLIKELY(!impl))
    return *this;
  RefPtr<StringImpl> new_impl = impl->LowerASCII();
  if (LIKELY(new_impl == impl))
    return *this;
  return AtomicString(String(std::move(new_impl)));
}

AtomicString AtomicString::UpperASCII() const {
  StringImpl* impl = this->Impl();
  if (UNLIKELY(!impl))
    return *this;
  return AtomicString(impl->UpperASCII());
}

template <typename IntegerType>
static AtomicString IntegerToAtomicString(IntegerType input) {
  IntegerToStringConverter<IntegerType> converter(input);
  return AtomicString(converter.Characters8(), converter.length());
}

AtomicString AtomicString::Number(int number) {
  return IntegerToAtomicString(number);
}

AtomicString AtomicString::Number(unsigned number) {
  return IntegerToAtomicString(number);
}

AtomicString AtomicString::Number(long number) {
  return IntegerToAtomicString(number);
}

AtomicString AtomicString::Number(unsigned long number) {
  return IntegerToAtomicString(number);
}

AtomicString AtomicString::Number(long long number) {
  return IntegerToAtomicString(number);
}

AtomicString AtomicString::Number(unsigned long long number) {
  return IntegerToAtomicString(number);
}

AtomicString AtomicString::Number(double number, unsigned precision) {
  NumberToStringBuffer buffer;
  return AtomicString(NumberToFixedPrecisionString(number, precision, buffer));
}

std::ostream& operator<<(std::ostream& out, const AtomicString& s) {
  return out << s.GetString();
}

#ifndef NDEBUG
void AtomicString::Show() const {
  string_.Show();
}
#endif

}  // namespace WTF
