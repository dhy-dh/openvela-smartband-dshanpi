#ifndef _SHELL_TASK_RUNNER_H_
#define _SHELL_TASK_RUNNER_H_

#include <functional>
#include <memory>

namespace shell {

class TaskRunner {
public:
    TaskRunner() {};
    virtual ~TaskRunner() = default;

    virtual void PostTask(std::function<void()> task) = 0;
};

class TaskQueue {
public:
    TaskQueue() { }
    virtual ~TaskQueue() { }

    virtual std::shared_ptr<TaskRunner> GetTaskRunner() = 0;
};

}

// TODO
#include <uv.h>
namespace shell {
std::unique_ptr<TaskQueue> MakeTaskQueueFromUV(uv_loop_t* loop);
}

#endif
