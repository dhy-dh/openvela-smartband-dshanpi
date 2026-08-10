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
#ifndef ASH_MESSAGE_LOOP_MESSAGE_QUEUE_H_
#define ASH_MESSAGE_LOOP_MESSAGE_QUEUE_H_

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include "ash/message_loop/message_pump.h"
#include "ash/task_runner/task_runner.h"

namespace ash {

class MessageQueue {
 public:
  MessageQueue();
  ~MessageQueue();

  void PostDelayedTask(OnceClosure task, Duration delay);

  void Startup(MessagePump* pump);
  void Shutdown();

 private:
  struct Task {
    Time time;
    OnceClosure task;
  };

  struct TaskComparator {
    bool operator()(const Task& lhs, const Task& rhs) const;
  };

  void ScheduleWork();
  Duration Drive();

  std::size_t GetTaskSize();

  MessagePump* pump_;
  std::priority_queue<Task, std::vector<Task>, TaskComparator> tasks_;
  Time next_schedule_time_;
  std::mutex mutex_;

  friend class MessagePump;
};

}  // namespace ash

#endif  // ASH_MESSAGE_LOOP_MESSAGE_QUEUE_H_
