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
#ifndef ASH_TIMER_TIMER_H_
#define ASH_TIMER_TIMER_H_

#include <memory>
#include <variant>
#include "ash/functional/closure.h"
#include "ash/functional/fn_once.h"
#include "ash/memory/weak_ptr.h"
#include "ash/task_runner/task_runner.h"
#include "ash/time/duration.h"

namespace ash {

class Timer : public SupportsWeakPtr<Timer> {
 public:
  static std::unique_ptr<Timer> Create(OnceClosure task, Duration timeout);
  static std::unique_ptr<Timer> CreateInterval(Closure task, Duration timeout);
  bool IsInterval();

 private:
  using Task = std::variant<OnceClosure, Closure>;
  Timer(Task task, Duration timeout);
  void Start(WeakPtr<Timer> weak);
  void OnTimerFired(WeakPtr<Timer> weak);

  Task task_;
  Duration timeout_;
  std::shared_ptr<TaskRunner> task_runner_;
  ASH_DISALLOW_COPY_AND_MOVE(Timer);
};

}  // namespace ash

#endif