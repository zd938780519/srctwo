// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/prefs/in_memory_pref_store.h"

#include "base/memory/ptr_util.h"
#include "base/test/scoped_task_environment.h"
#include "base/values.h"
#include "components/prefs/persistent_pref_store_unittest.h"
#include "components/prefs/pref_store_observer_mock.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {
const char kTestPref[] = "test.pref";

class InMemoryPrefStoreTest : public testing::Test {
 public:
  InMemoryPrefStoreTest() { }

  void SetUp() override { store_ = new InMemoryPrefStore(); }
 protected:
  base::test::ScopedTaskEnvironment scoped_task_environment_;
  scoped_refptr<InMemoryPrefStore> store_;
  PrefStoreObserverMock observer_;
};

TEST_F(InMemoryPrefStoreTest, SetGetValue) {
  const base::Value* value = NULL;
  base::Value* mutable_value = NULL;
  EXPECT_FALSE(store_->GetValue(kTestPref, &value));
  EXPECT_FALSE(store_->GetMutableValue(kTestPref, &mutable_value));

  store_->SetValue(kTestPref, base::MakeUnique<base::Value>(42),
                   WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  EXPECT_TRUE(store_->GetValue(kTestPref, &value));
  EXPECT_TRUE(base::Value(42).Equals(value));
  EXPECT_TRUE(store_->GetMutableValue(kTestPref, &mutable_value));
  EXPECT_TRUE(base::Value(42).Equals(mutable_value));

  store_->RemoveValue(kTestPref, WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  EXPECT_FALSE(store_->GetValue(kTestPref, &value));
  EXPECT_FALSE(store_->GetMutableValue(kTestPref, &mutable_value));
}

TEST_F(InMemoryPrefStoreTest, GetSetObserver) {
  // Starts with no observers.
  EXPECT_FALSE(store_->HasObservers());

  // Add one.
  store_->AddObserver(&observer_);
  EXPECT_TRUE(store_->HasObservers());

  // Remove only observer.
  store_->RemoveObserver(&observer_);
  EXPECT_FALSE(store_->HasObservers());
}

TEST_F(InMemoryPrefStoreTest, CallObserver) {
  // With observer included.
  store_->AddObserver(&observer_);

  // Triggers on SetValue.
  store_->SetValue(kTestPref, base::MakeUnique<base::Value>(42),
                   WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  observer_.VerifyAndResetChangedKey(kTestPref);

  // And RemoveValue.
  store_->RemoveValue(kTestPref, WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  observer_.VerifyAndResetChangedKey(kTestPref);

  // But not SetValueSilently.
  store_->SetValueSilently(kTestPref, base::MakeUnique<base::Value>(42),
                           WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  EXPECT_EQ(0u, observer_.changed_keys.size());

  // On multiple RemoveValues only the first one triggers observer.
  store_->RemoveValue(kTestPref, WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  observer_.VerifyAndResetChangedKey(kTestPref);
  store_->RemoveValue(kTestPref, WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  EXPECT_EQ(0u, observer_.changed_keys.size());

  // Doesn't make call on removed observers.
  store_->RemoveObserver(&observer_);
  store_->SetValue(kTestPref, base::MakeUnique<base::Value>(42),
                   WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  store_->RemoveValue(kTestPref, WriteablePrefStore::DEFAULT_PREF_WRITE_FLAGS);
  EXPECT_EQ(0u, observer_.changed_keys.size());
}

TEST_F(InMemoryPrefStoreTest, Initialization) {
  EXPECT_TRUE(store_->IsInitializationComplete());
}

TEST_F(InMemoryPrefStoreTest, ReadOnly) {
  EXPECT_FALSE(store_->ReadOnly());
}

TEST_F(InMemoryPrefStoreTest, GetReadError) {
  EXPECT_EQ(PersistentPrefStore::PREF_READ_ERROR_NONE, store_->GetReadError());
}

TEST_F(InMemoryPrefStoreTest, ReadPrefs) {
  EXPECT_EQ(PersistentPrefStore::PREF_READ_ERROR_NONE, store_->ReadPrefs());
}

TEST_F(InMemoryPrefStoreTest, CommitPendingWriteWithCallback) {
  TestCommitPendingWriteWithCallback(store_.get(), &scoped_task_environment_);
}

}  // namespace
