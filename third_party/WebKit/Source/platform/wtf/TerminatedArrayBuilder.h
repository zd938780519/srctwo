// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef TerminatedArrayBuilder_h
#define TerminatedArrayBuilder_h

#include "platform/wtf/Allocator.h"

namespace WTF {

template <typename T, template <typename> class ArrayType = TerminatedArray>
class TerminatedArrayBuilder {
  STACK_ALLOCATED();
  WTF_MAKE_NONCOPYABLE(TerminatedArrayBuilder);

 public:
  explicit TerminatedArrayBuilder(
      typename ArrayType<T>::Allocator::PassPtr array)
      : array_(array), count_(0), capacity_(0) {
    if (!array_)
      return;
    capacity_ = count_ = array_->size();
    DCHECK(array_->at(count_ - 1).IsLastInArray());
  }

  void Grow(size_t count) {
    DCHECK(count);
    if (!array_) {
      DCHECK(!count_);
      DCHECK(!capacity_);
      capacity_ = count;
      array_ = ArrayType<T>::Allocator::Create(capacity_);
    } else {
      DCHECK(array_->at(count_ - 1).IsLastInArray());
      capacity_ += count;
      array_ = ArrayType<T>::Allocator::Resize(
          ArrayType<T>::Allocator::Release(array_), capacity_);
      array_->at(count_ - 1).SetLastInArray(false);
    }
    array_->at(capacity_ - 1).SetLastInArray(true);
  }

  void Append(const T& item) {
    CHECK_LT(count_, capacity_);
    DCHECK(!item.IsLastInArray());
    array_->at(count_++) = item;
    if (count_ == capacity_)
      array_->at(capacity_ - 1).SetLastInArray(true);
  }

  typename ArrayType<T>::Allocator::PassPtr Release() {
    CHECK_EQ(count_, capacity_);
    AssertValid();
    return ArrayType<T>::Allocator::Release(array_);
  }

 private:
#if DCHECK_IS_ON()
  void AssertValid() {
    for (size_t i = 0; i < count_; ++i) {
      bool is_last_in_array = (i + 1 == count_);
      DCHECK_EQ(array_->at(i).IsLastInArray(), is_last_in_array);
    }
  }
#else
  void AssertValid() {}
#endif

  typename ArrayType<T>::Allocator::Ptr array_;
  size_t count_;
  size_t capacity_;
};

}  // namespace WTF

using WTF::TerminatedArrayBuilder;

#endif  // TerminatedArrayBuilder_h
