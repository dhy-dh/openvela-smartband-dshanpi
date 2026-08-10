#include "ash/task_runner/ctask_runner.h"
#include "ash/task_runner/task_runner.h"

extern "C" {
void TaskRunner_PostTask(TaskRunner* task_runner, Task task, void* user_data) {
  auto* runner =
      reinterpret_cast<std::shared_ptr<ash::TaskRunner>*>(task_runner);
  (*runner)->PostTask([task, user_data]() { task(user_data); });
}

void TaskRunner_PostTaskWithDeleter(TaskRunner* task_runner,
                                    Task task,
                                    void* user_data,
                                    Deleter deleter) {
  auto* runner =
      reinterpret_cast<std::shared_ptr<ash::TaskRunner>*>(task_runner);
  auto context = std::unique_ptr<void, Deleter>(user_data, deleter);
  (*runner)->PostTask(
      [task, context = std::move(context)]() { task(context.get()); });
}

void TaskRunner_PostDelayedTask(TaskRunner* task_runner,
                                Task task,
                                void* user_data,
                                int64_t delay) {
  auto* runner =
      reinterpret_cast<std::shared_ptr<ash::TaskRunner>*>(task_runner);
  (*runner)->PostDelayedTask([task, user_data]() { task(user_data); },
                             ash::Duration::FromMicro(delay));
}

void TaskRunner_PostDelayedTaskWithDeleter(TaskRunner* task_runner,
                                           Task task,
                                           void* user_data,
                                           Deleter deleter,
                                           int64_t delay) {
  auto* runner =
      reinterpret_cast<std::shared_ptr<ash::TaskRunner>*>(task_runner);
  auto context = std::unique_ptr<void, Deleter>(user_data, deleter);
  (*runner)->PostDelayedTask(
      [task, context = std::move(context)]() { task(context.get()); },
      ash::Duration::FromMicro(delay));
}

TaskRunner* TaskRunner_Clone(TaskRunner* task_runner) {
  auto* runner = new std::shared_ptr<ash::TaskRunner>(
      *reinterpret_cast<std::shared_ptr<ash::TaskRunner>*>(task_runner));
  return reinterpret_cast<TaskRunner*>(runner);
}

void TaskRunner_Release(TaskRunner* task_runner) {
  delete reinterpret_cast<std::shared_ptr<ash::TaskRunner>*>(task_runner);
}
}
