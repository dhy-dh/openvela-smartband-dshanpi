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
#ifndef ASH_THREADING_THREAD_H_
#define ASH_THREADING_THREAD_H_

#include <pthread.h>
#include "ash/macros/disallow_copy.h"
#include "ash/message_loop/message_queue.h"
#include "ash/task_runner/task_runner.h"

namespace ash {

class Thread {
 public:
  Thread();
  ~Thread();

  std::shared_ptr<TaskRunner> GetTaskRunner();
  void Quit();

 private:
  static void* Run(void* arg);

  std::shared_ptr<TaskRunner> task_runner_;
  ASH_DISALLOW_COPY_AND_MOVE(Thread);
};

}  // namespace ash

#endif  // ASH_THREADING_THREAD_H_
