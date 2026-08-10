/*
 * Copyright (C) 2025 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "ash/message_loop/message_loop.h"
#include "ash/logging/logging.h"
#include "ash/macros/compiler_macros.h"
#include "ash/memory/thread_local.h"
#include "ash/message_loop/message_pump_android.h"
#include "ash/message_loop/message_pump_impl.h"
#include "ash/message_loop/message_pump_uv.h"
#include "ash/message_loop/message_queue_runner.h"

namespace ash {

namespace {

THREAD_LOCAL(MessageLoop*) current = nullptr;

}

MessageLoop::MessageLoop(std::unique_ptr<MessagePump> pump,
                         std::shared_ptr<MessageQueue> queue)
    : pump_(std::move(pump)),
      queue_(queue),
      task_runner_(std::make_shared<MessageQueueRunner>(queue)) {
  ASH_CHECK_EQ(current.Get(), nullptr);
  current.Get() = this;
  pump_->queue_ = queue_.get();
  queue_->Startup(pump_.get());
}

MessageLoop::~MessageLoop() {
  queue_->Shutdown();
  ASH_CHECK_EQ(current.Get(), this);
  current.Get() = nullptr;
}

std::shared_ptr<TaskRunner> MessageLoop::GetTaskRunner() {
  return task_runner_;
}

MessageLoop* MessageLoop::Current() {
  return current.Get();
}

void MessageLoop::Run() {
  ASH_CHECK_EQ(current.Get(), this);
  pump_->Run();
}

void MessageLoop::Quit() {
  ASH_CHECK_EQ(current.Get(), this);
  pump_->Quit();
}

void MessageLoop::WatchFD(int fd,
                          FDWatchCB on_can_read,
                          FDWatchCB on_can_write,
                          FDWatchCB on_error) {
  ASH_CHECK_EQ(current.Get(), this);
  pump_->WatchFD(fd, std::move(on_can_read), std::move(on_can_write),
                 std::move(on_error));
}

void MessageLoop::UnwatchFD(int fd) {
  ASH_CHECK_EQ(current.Get(), this);
  pump_->UnwatchFD(fd);
}

void MessageLoop::AddListener(MessageLoopListener* listener) {
  ASH_CHECK_EQ(current.Get(), this);
  pump_->AddListener(listener);
}

void MessageLoop::RemoveListener(MessageLoopListener* listener) {
  ASH_CHECK_EQ(current.Get(), this);
  pump_->RemoveListener(listener);
}

std::unique_ptr<MessageLoop> MessageLoop::Create() {
  return CreateWithQueue(std::make_shared<MessageQueue>());
}

std::unique_ptr<MessageLoop> MessageLoop::CreateWithQueue(
    std::shared_ptr<MessageQueue> queue) {
  return std::make_unique<MessageLoop>(std::make_unique<MessagePumpImpl>(),
                                       std::move(queue));
}

#if defined(ASH_OS_NUTTX)
std::unique_ptr<MessageLoop> MessageLoop::CreateForUV(uv_loop_t* uv_loop) {
  return CreateForUVWithQueue(std::make_shared<MessageQueue>(), uv_loop);
}

std::unique_ptr<MessageLoop> MessageLoop::CreateForUVWithQueue(
    std::shared_ptr<MessageQueue> queue,
    uv_loop_t* uv_loop) {
  return std::make_unique<MessageLoop>(std::make_unique<MessagePumpUV>(uv_loop),
                                       std::move(queue));
}
#endif  // defined(ASH_OS_NUTTX)

#if defined(ASH_OS_ANDROID)
std::unique_ptr<MessageLoop> MessageLoop::CreateForAndroid(ALooper* looper) {
  return CreateForAndroidWithQueue(std::make_shared<MessageQueue>(), looper);
}

std::unique_ptr<MessageLoop> MessageLoop::CreateForAndroidWithQueue(
    std::shared_ptr<MessageQueue> queue,
    ALooper* looper) {
  return std::make_unique<MessageLoop>(
      std::make_unique<MessagePumpAndroid>(looper), std::move(queue));
}
#endif  // defined(ASH_OS_ANDROID)

#if defined(ASH_OS_NUTTX)
uv_loop_t* MessageLoop::GetUVLoop() {
  return pump_ ? pump_->GetUVLoop() : nullptr;
};
#endif  // defined(ASH_OS_NUTTX)
}  // namespace ash
