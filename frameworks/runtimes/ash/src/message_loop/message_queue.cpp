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
#include "ash/message_loop/message_queue.h"
#include "ash/trace_event/trace_event.h"

namespace ash {

MessageQueue::MessageQueue()
    : pump_(nullptr), next_schedule_time_(Time::Forever()) {}

MessageQueue::~MessageQueue() = default;

void MessageQueue::PostDelayedTask(OnceClosure task, Duration delay) {
  Time time = Time::Now() + delay;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    tasks_.push({time, std::move(task)});
    if (tasks_.top().time < time)
      return;
  }
  ScheduleWork();
}

void MessageQueue::Startup(MessagePump* pump) {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    pump_ = pump;
  }
  ScheduleWork();
}

void MessageQueue::Shutdown() {
  std::lock_guard<std::mutex> lock(mutex_);
  pump_ = nullptr;
}

void MessageQueue::ScheduleWork() {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!pump_ || tasks_.empty())
    return;
  pump_->Schedule();
}

Duration MessageQueue::Drive() {
  SCOPED_TRACE_EVENT("MessageQueue::Drive");
  Time now = Time::Now();
  std::unique_lock<std::mutex> lock(mutex_);
  next_schedule_time_ = Time::Forever();
  while (!tasks_.empty() && tasks_.top().time <= now) {
    OnceClosure task = std::move(const_cast<Task&>(tasks_.top()).task);
    tasks_.pop();
    lock.unlock();
    {
      SCOPED_TRACE_EVENT("MessageQueue::RunTask");
      std::move(task)();
    }
    lock.lock();
  };
  return tasks_.empty() ? Duration::Infinity() : tasks_.top().time - now;
}

bool MessageQueue::TaskComparator::operator()(const Task& lhs,
                                              const Task& rhs) const {
  return lhs.time > rhs.time;
}

std::size_t MessageQueue::GetTaskSize() {
  std::unique_lock<std::mutex> lock(mutex_);
  return tasks_.size();
}

}  // namespace ash
