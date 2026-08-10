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

#include "ash/timer/timer.h"
#include <strings.h>
#include "ash/message_loop/message_loop.h"

namespace ash {

Timer::Timer(Task task, Duration timeout)
    : SupportsWeakPtr<Timer>(this),
      task_(std::move(task)),
      timeout_(timeout),
      task_runner_(MessageLoop::Current()->GetTaskRunner()) {
  WeakPtr<Timer> weak = AsWeakPtr();
  Start(weak);
}

std::unique_ptr<Timer> Timer::Create(OnceClosure task, Duration timeout) {
  return std::unique_ptr<Timer>(new Timer(std::move(task), timeout));
}

std::unique_ptr<Timer> Timer::CreateInterval(Closure task, Duration timeout) {
  return std::unique_ptr<Timer>(new Timer(std::move(task), timeout));
}

void Timer::Start(WeakPtr<Timer> weak) {
  task_runner_->PostDelayedTask(
      [weak] {
        Timer* timer = weak.Get();
        if (timer == nullptr)
          return;
        timer->OnTimerFired(weak);
      },
      timeout_);
}

void Timer::OnTimerFired(WeakPtr<Timer> weak) {
  if (task_.index() == 0) {
    std::move(std::get<OnceClosure>(task_))();
  } else {
    std::get<Closure>(task_)();
    Start(weak);
  }
}

bool Timer::IsInterval() {
  if (task_.index() == 0)
    return false;
  else
    return true;
}

}  // namespace ash