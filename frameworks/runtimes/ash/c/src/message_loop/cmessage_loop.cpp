#include "ash/message_loop/cmessage_loop.h"
#include "ash/message_loop/message_loop.h"
#include "ash/message_loop/message_queue.h"
#include "ash/task_runner/ctask_runner.h"
#include "ash/task_runner/task_runner.h"

extern "C" {

MessageLoop* MessageLoop_Create() {
  return reinterpret_cast<MessageLoop*>(ash::MessageLoop::Create().release());
}

void MessageLoop_Destroy(MessageLoop* loop) {
  auto hold = std::unique_ptr<ash::MessageLoop>(
      reinterpret_cast<ash::MessageLoop*>(loop));
}

MessageLoop* MessageLoop_CreateWithQueue(MessageQueue* queue) {
  auto* shared_queue =
      reinterpret_cast<std::shared_ptr<ash::MessageQueue>*>(queue);
  return reinterpret_cast<MessageLoop*>(
      ash::MessageLoop::CreateWithQueue(*shared_queue).release());
}

#if defined(ASH_OS_NUTTX)
MessageLoop* MessageLoop_CreateForUV(uv_loop_t* uv_loop) {
  return reinterpret_cast<MessageLoop*>(
      ash::MessageLoop::CreateForUV(uv_loop).release());
}

MessageLoop* MessageLoop_CreateForUVWithQueue(MessageQueue* queue,
                                              uv_loop_t* uv_loop) {
  auto* shared_queue =
      reinterpret_cast<std::shared_ptr<ash::MessageQueue>*>(queue);
  return reinterpret_cast<MessageLoop*>(
      ash::MessageLoop::CreateForUVWithQueue(*shared_queue, uv_loop).release());
}

uv_loop_t* MessageLoop_GetUVLoop(MessageLoop* loop) {
  return reinterpret_cast<ash::MessageLoop*>(loop)->GetUVLoop();
}
#endif  // defined(ASH_OS_NUTTX)

#if defined(ASH_OS_ANDROID)
MessageLoop* CreateForAndroid(ALooper* looper) {
  return reinterpret_cast<MessageLoop*>(
      ash::MessageLoop::CreateForAndroid(looper).release());
}

MessageLoop* CreateForAndroidWithQueue(MessageQueue* queue, ALooper* looper) {
  auto* shared_queue =
      reinterpret_cast<std::shared_ptr<ash::MessageQueue>*>(queue);
  return reinterpret_cast<MessageLoop*>(
      ash::MessageLoop::CreateForAndroidWithQueue(*shared_queue, looper)
          .release());
}
#endif  // defined(ASH_OS_ANDROID)

void MessageLoop_Run(MessageLoop* loop) {
  reinterpret_cast<ash::MessageLoop*>(loop)->Run();
}
void MessageLoop_Quit(MessageLoop* loop) {
  reinterpret_cast<ash::MessageLoop*>(loop)->Quit();
}

TaskRunner* MessageLoop_GetTaskRunner(MessageLoop* loop) {
  auto task_runner = new std::shared_ptr<ash::TaskRunner>(
      reinterpret_cast<ash::MessageLoop*>(loop)->GetTaskRunner());
  return reinterpret_cast<TaskRunner*>(task_runner);
}

MessageLoop* MessageLoop_Current() {
  return reinterpret_cast<MessageLoop*>(ash::MessageLoop::Current());
}

void MessageLoop_WatchFD(MessageLoop* loop,
                         int fd,
                         FDWatchCB on_can_read,
                         FDWatchCB on_can_write,
                         FDWatchCB on_error) {
  auto message_loop = reinterpret_cast<ash::MessageLoop*>(loop);
  message_loop->WatchFD(fd, ash::MessageLoop::FDWatchCB(on_can_read),
                        ash::MessageLoop::FDWatchCB(on_can_write),
                        ash::MessageLoop::FDWatchCB(on_error));
}

void MessageLoop_UnwatchFD(MessageLoop* loop, int fd) {
  auto message_loop = reinterpret_cast<ash::MessageLoop*>(loop);
  message_loop->UnwatchFD(fd);
}
}
