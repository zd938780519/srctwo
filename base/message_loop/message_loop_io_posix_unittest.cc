// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/message_loop/message_loop.h"

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/files/file_util.h"
#include "base/files/scoped_file.h"
#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/posix/eintr_wrapper.h"
#include "base/run_loop.h"
#include "base/test/gtest_util.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {

#if !defined(OS_NACL)

namespace {

class MessageLoopForIoPosixTest : public testing::Test {
 public:
  MessageLoopForIoPosixTest() {}

  // testing::Test interface.
  void SetUp() override {
    // Create a file descriptor.  Doesn't need to be readable or writable,
    // as we don't need to actually get any notifications.
    // pipe() is just the easiest way to do it.
    int pipefds[2];
    int err = pipe(pipefds);
    ASSERT_EQ(0, err);
    read_fd_ = ScopedFD(pipefds[0]);
    write_fd_ = ScopedFD(pipefds[1]);
  }

  void TriggerReadEvent() {
    // Write from the other end of the pipe to trigger the event.
    char c = '\0';
    EXPECT_EQ(1, HANDLE_EINTR(write(write_fd_.get(), &c, 1)));
  }

 protected:
  ScopedFD read_fd_;
  ScopedFD write_fd_;

  DISALLOW_COPY_AND_ASSIGN(MessageLoopForIoPosixTest);
};

class TestHandler : public MessageLoopForIO::Watcher {
 public:
  void OnFileCanReadWithoutBlocking(int fd) override {
    watcher_to_delete_ = nullptr;
    is_readable_ = true;
    RunLoop::QuitCurrentWhenIdleDeprecated();
  }
  void OnFileCanWriteWithoutBlocking(int fd) override {
    watcher_to_delete_ = nullptr;
    is_writable_ = true;
    RunLoop::QuitCurrentWhenIdleDeprecated();
  }

  bool is_readable_ = false;
  bool is_writable_ = false;

  // If set then the contained watcher will be deleted on notification.
  std::unique_ptr<MessageLoopForIO::FileDescriptorWatcher> watcher_to_delete_;
};

// Watcher that calls specified closures when read/write events occur. Verifies
// that each non-null closure passed to this class is called once and only once.
// Also resets the read event by reading from the FD.
class CallClosureHandler : public MessageLoopForIO::Watcher {
 public:
  CallClosureHandler(OnceClosure read_closure, OnceClosure write_closure)
      : read_closure_(std::move(read_closure)),
        write_closure_(std::move(write_closure)) {}

  ~CallClosureHandler() override {
    EXPECT_TRUE(read_closure_.is_null());
    EXPECT_TRUE(write_closure_.is_null());
  }

  void SetReadClosure(OnceClosure read_closure) {
    EXPECT_TRUE(read_closure_.is_null());
    read_closure_ = std::move(read_closure);
  }

  void SetWriteClosure(OnceClosure write_closure) {
    EXPECT_TRUE(write_closure_.is_null());
    write_closure_ = std::move(write_closure);
  }

  // base:MessagePumpFuchsia::Watcher interface.
  void OnFileCanReadWithoutBlocking(int fd) override {
    // Empty the pipe buffer to reset the event. Otherwise libevent
    // implementation of MessageLoop may call the event handler again even if
    // |read_closure_| below quits the RunLoop.
    char c;
    int result = HANDLE_EINTR(read(fd, &c, 1));
    if (result == -1) {
      PLOG(ERROR) << "read";
      FAIL();
    }
    EXPECT_EQ(result, 1);

    ASSERT_FALSE(read_closure_.is_null());
    std::move(read_closure_).Run();
  }

  void OnFileCanWriteWithoutBlocking(int fd) override {
    ASSERT_FALSE(write_closure_.is_null());
    std::move(write_closure_).Run();
  }

 private:
  OnceClosure read_closure_;
  OnceClosure write_closure_;
};

TEST_F(MessageLoopForIoPosixTest, FileDescriptorWatcherOutlivesMessageLoop) {
  // Simulate a MessageLoop that dies before an FileDescriptorWatcher.
  // This could happen when people use the Singleton pattern or atexit.

  // Arrange for watcher to live longer than message loop.
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);
  TestHandler handler;
  {
    MessageLoopForIO message_loop;

    message_loop.WatchFileDescriptor(write_fd_.get(), true,
                                     MessageLoopForIO::WATCH_WRITE, &watcher,
                                     &handler);
    // Don't run the message loop, just destroy it.
  }

  ASSERT_FALSE(handler.is_readable_);
  ASSERT_FALSE(handler.is_writable_);
}

TEST_F(MessageLoopForIoPosixTest, FileDescriptorWatcherDoubleStop) {
  // Verify that it's ok to call StopWatchingFileDescriptor().
  // (Errors only showed up in valgrind.)

  // Arrange for message loop to live longer than watcher.
  MessageLoopForIO message_loop;
  {
    MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);

    TestHandler handler;
    message_loop.WatchFileDescriptor(write_fd_.get(), true,
                                     MessageLoopForIO::WATCH_WRITE, &watcher,
                                     &handler);
    ASSERT_TRUE(watcher.StopWatchingFileDescriptor());
    ASSERT_TRUE(watcher.StopWatchingFileDescriptor());
  }
}

TEST_F(MessageLoopForIoPosixTest, FileDescriptorWatcherDeleteInCallback) {
  // Verify that it is OK to delete the FileDescriptorWatcher from within a
  // callback.
  MessageLoopForIO message_loop;

  TestHandler handler;
  handler.watcher_to_delete_ =
      std::make_unique<MessageLoopForIO::FileDescriptorWatcher>(FROM_HERE);

  message_loop.WatchFileDescriptor(write_fd_.get(), true,
                                   MessageLoopForIO::WATCH_WRITE,
                                   handler.watcher_to_delete_.get(), &handler);
  RunLoop().Run();
}

// Verify that basic readable notification works.
TEST_F(MessageLoopForIoPosixTest, WatchReadable) {
  MessageLoopForIO message_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);
  TestHandler handler;

  // Watch the pipe for readability.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      read_fd_.get(), /*persistent=*/false, MessageLoopForIO::WATCH_READ,
      &watcher, &handler));

  // The pipe should not be readable when first created.
  RunLoop().RunUntilIdle();
  ASSERT_FALSE(handler.is_readable_);
  ASSERT_FALSE(handler.is_writable_);

  TriggerReadEvent();

  // We don't want to assume that the read fd becomes readable the
  // instant a bytes is written, so Run until quit by an event.
  RunLoop().Run();

  ASSERT_TRUE(handler.is_readable_);
  ASSERT_FALSE(handler.is_writable_);
}

// Verify that watching a file descriptor for writability succeeds.
TEST_F(MessageLoopForIoPosixTest, WatchWritable) {
  MessageLoopForIO message_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);
  TestHandler handler;

  // Watch the pipe for writability.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      write_fd_.get(), /*persistent=*/false, MessageLoopForIO::WATCH_WRITE,
      &watcher, &handler));

  // We should not receive a writable notification until we process events.
  ASSERT_FALSE(handler.is_readable_);
  ASSERT_FALSE(handler.is_writable_);

  // The pipe should be writable immediately, but wait for the quit closure
  // anyway, to be sure.
  RunLoop().Run();

  ASSERT_FALSE(handler.is_readable_);
  ASSERT_TRUE(handler.is_writable_);
}

void StopWatching(MessageLoopForIO::FileDescriptorWatcher* controller,
                  RunLoop* run_loop) {
  controller->StopWatchingFileDescriptor();
  run_loop->Quit();
}

// Verify that StopWatchingFileDescriptor() works from an event handler.
TEST_F(MessageLoopForIoPosixTest, StopFromHandler) {
  MessageLoopForIO message_loop;
  RunLoop run_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);
  CallClosureHandler handler(BindOnce(&StopWatching, &watcher, &run_loop),
                             OnceClosure());

  // Create persistent watcher.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      read_fd_.get(), /*persistent=*/true, MessageLoopForIO::WATCH_READ,
      &watcher, &handler));

  TriggerReadEvent();
  run_loop.Run();

  // Trigger the event again. The event handler should not be called again.
  TriggerReadEvent();
  RunLoop().RunUntilIdle();
}

// Verify that non-persistent watcher is called only once.
TEST_F(MessageLoopForIoPosixTest, NonPersistentWatcher) {
  MessageLoopForIO message_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);

  RunLoop run_loop;
  CallClosureHandler handler(run_loop.QuitClosure(), OnceClosure());

  // Create a non-persistent watcher.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      read_fd_.get(), /*persistent=*/false, MessageLoopForIO::WATCH_READ,
      &watcher, &handler));

  TriggerReadEvent();
  run_loop.Run();

  // Trigger the event again. handler should not be called again.
  TriggerReadEvent();
  RunLoop().RunUntilIdle();
}

// Verify that persistent watcher is called every time the event is triggered.
TEST_F(MessageLoopForIoPosixTest, PersistentWatcher) {
  MessageLoopForIO message_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);

  RunLoop run_loop1;
  CallClosureHandler handler(run_loop1.QuitClosure(), OnceClosure());

  // Create persistent watcher.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      read_fd_.get(), /*persistent=*/true, MessageLoopForIO::WATCH_READ,
      &watcher, &handler));

  TriggerReadEvent();
  run_loop1.Run();

  RunLoop run_loop2;
  handler.SetReadClosure(run_loop2.QuitClosure());

  // Trigger the event again. handler should be called now, which will quit
  // run_loop2.
  TriggerReadEvent();
  run_loop2.Run();
}

void StopWatchingAndWatchAgain(
    MessageLoopForIO::FileDescriptorWatcher* controller,
    int fd,
    MessageLoopForIO::Watcher* new_handler,
    RunLoop* run_loop) {
  controller->StopWatchingFileDescriptor();

  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      fd, /*persistent=*/true, MessageLoopForIO::WATCH_READ, controller,
      new_handler));

  run_loop->Quit();
}

// Verify that a watcher can be stopped and reused from an event handler.
TEST_F(MessageLoopForIoPosixTest, StopAndRestartFromHandler) {
  MessageLoopForIO message_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);

  RunLoop run_loop1;
  RunLoop run_loop2;
  CallClosureHandler handler2(run_loop2.QuitClosure(), OnceClosure());
  CallClosureHandler handler1(BindOnce(&StopWatchingAndWatchAgain, &watcher,
                                       read_fd_.get(), &handler2, &run_loop1),
                              OnceClosure());

  // Create persistent watcher.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      read_fd_.get(), /*persistent=*/true, MessageLoopForIO::WATCH_READ,
      &watcher, &handler1));

  TriggerReadEvent();
  run_loop1.Run();

  // Trigger the event again. handler2 should be called now, which will quit
  // run_loop2
  TriggerReadEvent();
  run_loop2.Run();
}

// Verify that the pump properly handles a delayed task after an IO event.
TEST_F(MessageLoopForIoPosixTest, IoEventThenTimer) {
  MessageLoopForIO message_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);

  RunLoop timer_run_loop;
  message_loop.task_runner()->PostDelayedTask(
      FROM_HERE, timer_run_loop.QuitClosure(),
      base::TimeDelta::FromMilliseconds(10));

  RunLoop watcher_run_loop;
  CallClosureHandler handler(watcher_run_loop.QuitClosure(), OnceClosure());

  // Create a non-persistent watcher.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      read_fd_.get(), /*persistent=*/false, MessageLoopForIO::WATCH_READ,
      &watcher, &handler));

  TriggerReadEvent();

  // Normally the IO event will be received before the delayed task is
  // executed, so this run loop will first handle the IO event and then quit on
  // the timer.
  timer_run_loop.Run();

  // Run watcher_run_loop in case the IO event wasn't received before the
  // delayed task.
  watcher_run_loop.Run();
}

// Verify that the pipe can handle an IO event after a delayed task.
TEST_F(MessageLoopForIoPosixTest, TimerThenIoEvent) {
  MessageLoopForIO message_loop;
  MessageLoopForIO::FileDescriptorWatcher watcher(FROM_HERE);

  // Trigger read event from a delayed task.
  message_loop.task_runner()->PostDelayedTask(
      FROM_HERE,
      BindOnce(&MessageLoopForIoPosixTest::TriggerReadEvent, Unretained(this)),
      TimeDelta::FromMilliseconds(1));

  RunLoop run_loop;
  CallClosureHandler handler(run_loop.QuitClosure(), OnceClosure());

  // Create a non-persistent watcher.
  ASSERT_TRUE(MessageLoopForIO::current()->WatchFileDescriptor(
      read_fd_.get(), /*persistent=*/false, MessageLoopForIO::WATCH_READ,
      &watcher, &handler));

  run_loop.Run();
}

}  // namespace

#endif  // !defined(OS_NACL)

}  // namespace base
