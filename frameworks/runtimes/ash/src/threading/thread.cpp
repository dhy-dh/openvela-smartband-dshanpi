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
#include "ash/threading/thread.h"
#include "ash/logging/logging.h"
#include "ash/macros/compiler_macros.h"
#include "ash/message_loop/message_loop.h"
#include "ash/message_loop/message_queue.h"
#include "ash/message_loop/message_queue_runner.h"

#define DEFAULT_QUICKAPP_THREADSTACKSIZE 131072
#define DEFAULT_QUICKAPP_PRIORITY 100

namespace ash {

Thread::Thread() {
  std::shared_ptr<MessageQueue> queue = std::make_shared<MessageQueue>();

  pthread_t th;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
#if defined(ASH_OS_NUTTX)
  pthread_attr_setstacksize(&attr, DEFAULT_QUICKAPP_THREADSTACKSIZE);
  attr.priority = DEFAULT_QUICKAPP_PRIORITY;
#endif  // defined(ASH_OS_NUTTX)

  int r = pthread_create(&th, &attr, &Thread::Run,
                         new std::shared_ptr<MessageQueue>(queue));
  ASH_CHECK(r == 0) << "Failed to create thread";

  task_runner_ = std::make_shared<MessageQueueRunner>(queue);
}

Thread::~Thread() = default;

std::shared_ptr<TaskRunner> Thread::GetTaskRunner() {
  return task_runner_;
}

void Thread::Quit() {
  task_runner_->PostTask([]() { MessageLoop::Current()->Quit(); });
}

void* Thread::Run(void* arg) {
  std::shared_ptr<MessageQueue>* queue =
      reinterpret_cast<std::shared_ptr<MessageQueue>*>(arg);
  std::unique_ptr<MessageLoop> message_loop =
      MessageLoop::CreateWithQueue(std::move(*queue));
  delete queue;
  message_loop->Run();
  return nullptr;
}

}  // namespace ash
