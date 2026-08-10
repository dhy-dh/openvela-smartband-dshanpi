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
#ifndef ASH_CTASK_RUNNER_TASK_RUNNER_H_
#define ASH_CTASK_RUNNER_TASK_RUNNER_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TaskRunner TaskRunner;
typedef void (*Task)(void*);
typedef void (*Deleter)(void*);

void TaskRunner_PostTask(TaskRunner* task_runner, Task task, void* user_data);

void TaskRunner_PostTaskWithDeleter(TaskRunner* task_runner,
                                    Task task,
                                    void* user_data,
                                    Deleter deleter);

void TaskRunner_PostDelayedTask(TaskRunner* task_runner,
                                Task task,
                                void* user_data,
                                int64_t delay);

void TaskRunner_PostDelayedTaskWithDeleter(TaskRunner* task_runner,
                                           Task task,
                                           void* user_data,
                                           Deleter deleter,
                                           int64_t delay);

TaskRunner* TaskRunner_Clone(TaskRunner* task_runner);

void TaskRunner_Release(TaskRunner* task_runner);

#ifdef __cplusplus
}
#endif

#endif  // ASH_CTASK_RUNNER_TASK_RUNNER_H_
