#ifndef _QUICKAPP_SHELL_UV_ASYNC_H_
#define _QUICKAPP_SHELL_UV_ASYNC_H_

#include "mutex_value.h"
#include "task_queue.h"
#include <atomic>
#include <uv.h>
#include <vector>

namespace shell {

struct TaskQueueData : public TaskRunner {
    std::atomic<bool> running;
    uv_async_t async;
    Mutex<std::vector<std::function<void()>>> queue;

    TaskQueueData()
        : running(true)
    {
    }

    void PostTask(std::function<void()> task) override
    {
        if (running.load()) {
            auto q = queue.lock();
            q->push_back(std::move(task));
            if (q->size() == 1) {
                uv_async_send(&async);
            }
        }
    }
};

class UVTaskQueue : public TaskQueue {
public:
    UVTaskQueue(uv_loop_t* loop)
    {
        queue_data_ = std::make_shared<TaskQueueData>();
        uv_async_init(loop, &(queue_data_->async), _run_task);
        queue_data_->async.data = new std::shared_ptr<TaskQueueData>(queue_data_);
    }
    ~UVTaskQueue()
    {
        if (queue_data_) {
            uv_close((uv_handle_t*)&(queue_data_->async), [](uv_handle_t* handle) {
                std::shared_ptr<TaskQueueData>* pptr = (std::shared_ptr<TaskQueueData>*)(handle->data);
                (*pptr)->running.store(false);
                handle->data = NULL;
                delete pptr;
            });
        }
    }

    std::shared_ptr<TaskRunner> GetTaskRunner() override
    {
        return std::static_pointer_cast<TaskRunner>(queue_data_);
    }

private:
    static void _run_task(uv_async_t* handle)
    {
        std::shared_ptr<TaskQueueData>* pptr = (std::shared_ptr<TaskQueueData>*)(handle->data);

        if (pptr) {
            std::vector<std::function<void()>> queue(std::move((*pptr)->queue.drop()));
            auto it = queue.begin();
            while (it != queue.end()) {
                (*it)();
                queue.erase(it);
            }
        }
    }

    std::shared_ptr<TaskQueueData> queue_data_;
};

std::unique_ptr<TaskQueue> MakeTaskQueueFromUV(uv_loop_t* loop)
{
    return std::make_unique<UVTaskQueue>(loop);
}

}

#endif
