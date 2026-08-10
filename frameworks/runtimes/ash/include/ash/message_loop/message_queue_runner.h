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
#ifndef ASH_MESSAGE_LOOP_MESSAGE_QUEUE_RUNNER_H_
#define ASH_MESSAGE_LOOP_MESSAGE_QUEUE_RUNNER_H_

#include <memory>
#include "ash/message_loop/message_queue.h"
#include "ash/task_runner/task_runner.h"

namespace ash {

class MessageQueueRunner : public TaskRunner {
 public:
  MessageQueueRunner(std::weak_ptr<MessageQueue> queue);
  ~MessageQueueRunner();

  void PostTask(OnceClosure task) override;
  void PostDelayedTask(OnceClosure task, Duration delay) override;

 private:
  std::weak_ptr<MessageQueue> queue_;
};

}  // namespace ash

#endif  // ASH_MESSAGE_LOOP_MESSAGE_QUEUE_RUNNER_H_
