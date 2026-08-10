# QuickApp API 参考文档

本文档详细说明 `include/` 目录下各公开头文件提供的类型定义与接口函数。

---

## 目录

- [quickapp.h — 应用核心 API](#quickapph--应用核心-api)
- [gui_widget_context.h — GUI 组件上下文](#gui_widget_contexth--gui-组件上下文)
- [qwrapper.h — 平台适配接口](#qwrapperh--平台适配接口)
- [quickapp_inspector.h — Inspector 调试接口](#quickapp_inspectorh--inspector-调试接口)
- [inspector_dump.h — 内存与性能诊断](#inspector_dumph--内存与性能诊断)

---

## quickapp.h — 应用核心 API

快应用框架的核心头文件，提供应用创建、生命周期管理、页面路由、Manifest 查询、运行状态获取等接口。

### 枚举类型

#### QAppAnimationFunction

页面切换动画类型。

| 值 | 说明 |
| --- | --- |
| `QAPP_ANIMATION_DEFAUT` | 默认动画 |
| `QAPP_ANIMATION_NONE` | 无动画 |
| `QAPP_ANIMATION_SLIDE` | 滑动动画 |

#### QAppPageAction

页面动作类型，用于 `QAppRouteInfo.page_animation` 数组索引。

| 值 | 说明 |
| --- | --- |
| `QAPP_PAGE_OPEN_ENTER` | 打开页面时的进入动画 |
| `QAPP_PAGE_CLOSE_ENTER` | 关闭页面时的进入动画 |
| `QAPP_PAGE_OPEN_EXIT` | 打开页面时的退出动画 |
| `QAPP_PAGE_CLOSE_EXIT` | 关闭页面时的退出动画 |

#### QAPP_EVENT 事件码

应用框架事件码，通过 `QApplicationClient.OnAppNotifyEvent` 回调通知。

| 值 | 说明 |
| --- | --- |
| `QAPP_EVENT_START` | 应用启动 |
| `QAPP_EVENT_BACKGROUND` | 应用进入后台 |
| `QAPP_EVENT_EXIT` | 应用退出，AMS 可释放资源 |
| `QAPP_EVENT_REQ_EXIT` | 应用请求退出，由 AMS 响应并关闭应用 |
| `QAPP_EVENT_OOM` | 内存不足 |
| `QAPP_EVENT_ENOSPC` | 磁盘空间不足 |
| `QAPP_EVENT_PREPARE` | 应用准备阶段 |
| `QAPP_EVENT_CREATE` | 应用创建 |
| `QAPP_EVENT_SHOW` | 应用显示 |
| `QAPP_EVENT_HIDE` | 应用隐藏 |
| `QAPP_EVENT_DESTROY` | 应用销毁 |
| `QAPP_EVENT_PAGE_BACKPRESS` | 页面返回键事件 |
| `QAPP_EVENT_FIRST_PAGE_SHOW` | 首页显示（可用于启动动画控制） |

#### QAppStatus

应用运行状态。

| 值 | 说明 |
| --- | --- |
| `QAPP_STATUS_UNKWON` | 未知状态 |
| `QAPP_STATUS_FOREGROUND` | 前台运行 |
| `QAPP_STATUS_SYSTEM_HOME` | 系统主页 |
| `QAPP_STATUS_BACKGROUND` | 后台运行 |

#### QAppPageLaunchMode

页面启动模式。

| 值 | 说明 |
| --- | --- |
| `QAPP_PAGE_LAUCH_STANDARD` | 标准模式，每次创建新实例 |
| `QAPP_PAGE_LAUCH_SINGLETASK` | 单任务模式，复用已有实例 |

### 结构体

#### QAppRouteInfo

页面路由信息。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `uri` | `const char*` | 路由地址，格式：`schema://<package>/<pagepath>?<querystring>` |
| `querystring` | `const char*` | 查询参数字符串 |
| `page_animation` | `int[QAPP_PAGE_ACTION_MAX]` | 各页面动作对应的动画类型（取值参考 `QAppAnimationFunction`） |
| `clear_task` | `bool` | 是否清除任务栈 |
| `extra_data` | `const char*` | 附加数据，JSON 格式 |

#### QApplicationClient

应用客户端回调接口，由集成方实现并传入 `QApplicationCreateInfo`。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `client_context` | `void*` | 客户端实例指针 |
| `OnAppNotifyEvent` | 函数指针 | 应用事件通知回调，`qapp_event` 取值参考事件码枚举 |
| `OnNavigateToApp` | 函数指针 | 跨应用导航回调，返回 `true` 表示已处理 |
| `OnAppDestroyOnUI` | 函数指针 | 应用在 UI 线程上销毁时的回调 |
| `OnFeatureManagerPrepare` | 函数指针 | Feature Manager 准备就绪回调 |
| `OnFeatureManagerDestroy` | 函数指针 | Feature Manager 销毁回调 |

#### QApplicationCreateInfo

应用创建参数。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `uri` | `const char*` | 应用 URL |
| `client` | `QApplicationClient*` | 客户端回调接口 |
| `gui_context` | `GUIWidgetContextHandle` | GUI 组件上下文句柄 |
| `message_loop` | `MessageLoop*` | 应用消息循环 |
| `ui_message_loop` | `MessageLoop*` | UI 消息循环 |
| `stack_size` | `size_t` | JS 栈大小 |
| `heap_size` | `size_t` | JS 堆大小 |
| `entry` | `const char*` | 入口文件路径 |
| `feature_manager_set` | 函数指针 | Feature Manager 设置回调 |
| `feature_register` | 函数指针 | Feature 注册回调 |
| `user_data` | `void*` | 用户自定义数据 |

#### QAppRunInfo

应用运行时信息。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `process_status` | `int` | 进程状态 |
| `memory_useage_bytes` | `uint64_t` | 内存使用量（字节） |
| `run_time_ms` | `uint64_t` | 运行时长（毫秒） |
| `hide_time_ms` | `uint64_t` | 后台时长（毫秒） |
| `priority` | `int` | 应用优先级 |

#### QAppPageInfo

页面栈信息。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `name` | `const char*` | 页面名称 |
| `path` | `const char*` | 页面路径 |
| `lanch_mode` | `int` | 启动模式（取值参考 `QAppPageLaunchMode`） |

### 应用生命周期接口

#### QApplicationCreate

```c
QApplicationHandle QApplicationCreate(const QApplicationCreateInfo* info);
```

创建快应用实例。

- `info`：应用创建参数，参考 `QApplicationCreateInfo`。
- 返回：应用句柄，失败返回 `NULL`。

#### QApplicationStart

```c
int QApplicationStart(QApplicationHandle app);
```

启动应用，开始执行 JS 入口文件并进入消息循环。

- `app`：应用句柄。
- 返回：0 表示成功，非 0 表示失败。

#### QApplicationRunOnce

```c
int QApplicationRunOnce(QApplicationHandle app);
```

执行一次应用消息循环迭代。适用于需要外部控制循环节奏的场景。

- `app`：应用句柄。
- 返回：0 表示成功。

#### QApplicationRequestExit

```c
void QApplicationRequestExit(QApplicationHandle app);
```

由外部调用方（如 AMS 或其他集成方）调用，请求应用执行退出流程。这是应用退出的统一入口，无论退出原因是什么，最终都应通过此接口触发。

- `app`：应用句柄。

#### QApplicationSendExitEvent

```c
void QApplicationSendExitEvent(QApplicationHandle app);
```

由应用内部调用，将退出事件（`QAPP_EVENT_EXIT`）通知给外部调用方。调用方收到后应调用 `QApplicationRequestExit` 回头请求应用退出，以保持退出逻辑的一致性。

- `app`：应用句柄。

> **说明：** 应用退出有两种场景：
> 1. 外部主动退出：调用方直接调用 `QApplicationRequestExit`。
> 2. 内部触发退出：应用先调用 `QApplicationSendExitEvent` 通知外部调用方，再由调用方调用 `QApplicationRequestExit` 完成退出。
>
> 这种设计确保所有退出路径都经过外部调用方统一管控，便于资源回收和状态同步。

#### QApplicationShow

```c
void QApplicationShow(QApplicationHandle app);
```

将应用切换到前台显示状态，触发 `QAPP_EVENT_SHOW` 事件。

- `app`：应用句柄。

#### QApplicationHide

```c
void QApplicationHide(QApplicationHandle app);
```

将应用切换到后台隐藏状态，触发 `QAPP_EVENT_HIDE` 事件。

- `app`：应用句柄。

#### QApplicationBackPress

```c
void QApplicationBackPress(QApplicationHandle app);
```

模拟返回键按下，触发 `QAPP_EVENT_PAGE_BACKPRESS` 事件。

- `app`：应用句柄。

#### QApplicationRunGC

```c
void QApplicationRunGC(QApplicationHandle app);
```

手动触发 JavaScript 引擎的垃圾回收。

- `app`：应用句柄。

#### QApplicationGetClient

```c
QApplicationClient* QApplicationGetClient(QApplicationHandle app);
```

获取应用关联的客户端回调接口。

- `app`：应用句柄。
- 返回：客户端回调结构体指针。

### 页面路由接口

#### QApplicationRoute

```c
int QApplicationRoute(QApplicationHandle app, const char* uri);
```

根据 URI 进行页面路由跳转。

- `app`：应用句柄。
- `uri`：目标页面 URI。
- 返回：0 表示成功。

#### QApplicationStackPagePush

```c
bool QApplicationStackPagePush(QApplicationHandle app, const QAppRouteInfo* route_info);
```

将新页面压入导航栈。

- `app`：应用句柄。
- `route_info`：路由信息。
- 返回：`true` 表示成功。

#### QApplicationStackPageBack

```c
bool QApplicationStackPageBack(QApplicationHandle app, const char* back_path);
```

页面回退。如果指定 `back_path`，则回退到该路径对应的页面。

- `app`：应用句柄。
- `back_path`：目标页面路径，`NULL` 表示回退一页。
- 返回：`true` 表示成功。

#### QApplicationStackPageReplace

```c
bool QApplicationStackPageReplace(QApplicationHandle app, const QAppRouteInfo* route_info);
```

替换当前栈顶页面。

- `app`：应用句柄。
- `route_info`：路由信息。
- 返回：`true` 表示成功。

#### QApplicationClearStackPages

```c
bool QApplicationClearStackPages(QApplicationHandle app);
```

清空页面导航栈。

- `app`：应用句柄。
- 返回：`true` 表示成功。

#### QApplicationGetStackPageLength

```c
size_t QApplicationGetStackPageLength(QApplicationHandle app);
```

获取当前页面栈深度。

- `app`：应用句柄。
- 返回：栈中页面数量。

#### QApplicationEnumStackPages

```c
void QApplicationEnumStackPages(QApplicationHandle app,
    bool (*enum_cb)(int stack_index, const QAppPageInfo* page_info, void* user_data),
    void* user_data);
```

遍历页面栈，栈顶页面优先枚举。回调返回 `false` 可提前终止遍历。

- `app`：应用句柄。
- `enum_cb`：枚举回调函数。
- `user_data`：用户自定义数据，透传给回调。

### 应用信息查询接口

#### QApplicationGetPackageName

```c
const char* QApplicationGetPackageName(QApplicationHandle app);
```

获取应用包名。

- `app`：应用句柄。
- 返回：包名字符串。

#### QApplicationGetPackagePath

```c
const char* QApplicationGetPackagePath(QApplicationHandle app, char* path, size_t max);
```

获取应用包所在路径。

- `app`：应用句柄。
- `path`：输出缓冲区。
- `max`：缓冲区最大长度。
- 返回：路径字符串。

#### QApplicationGetTempDir / QApplicationGetCacheDir / QApplicationGetDataDir

```c
const char* QApplicationGetTempDir(QApplicationHandle app, char* path, size_t max);
const char* QApplicationGetCacheDir(QApplicationHandle app, char* path, size_t max);
const char* QApplicationGetDataDir(QApplicationHandle app, char* path, size_t max);
```

分别获取应用的临时目录、缓存目录和数据目录路径。

- `app`：应用句柄。
- `path`：输出缓冲区。
- `max`：缓冲区最大长度。
- 返回：目录路径字符串。

#### QApplicationGetRunInfo

```c
bool QApplicationGetRunInfo(QApplicationHandle app, QAppRunInfo* pinfo);
```

获取应用运行时信息（内存占用、运行时长等）。

- `app`：应用句柄。
- `pinfo`：输出参数，参考 `QAppRunInfo`。
- 返回：`true` 表示成功。

#### QApplicationHasFeature

```c
bool QApplicationHasFeature(QApplicationHandle app, const char* feature);
```

检查应用是否具备指定 Feature 能力。

- `app`：应用句柄。
- `feature`：Feature 名称。
- 返回：`true` 表示具备。

#### QApplicationIsDeclearedFeature

```c
bool QApplicationIsDeclearedFeature(QApplicationHandle app, const char* feature);
```

检查应用 Manifest 中是否声明了指定 Feature。

- `app`：应用句柄。
- `feature`：Feature 名称。
- 返回：`true` 表示已声明。

#### QApplicationGetFeatureManager

```c
FeatureManagerHandle QApplicationGetFeatureManager(QApplicationHandle app);
```

获取应用的 Feature Manager 句柄。

- `app`：应用句柄。
- 返回：Feature Manager 句柄。

### Manifest 查询接口

#### QApplicationGetManifest

```c
QAppManifestHandle QApplicationGetManifest(QApplicationHandle app);
```

获取应用 Manifest 句柄。

- `app`：应用句柄。
- 返回：Manifest 句柄。

#### QAppManifestGetAppName

```c
const char* QAppManifestGetAppName(QAppManifestHandle hm, char* app_name, size_t max);
```

获取应用名称。

#### QAppManifestGetVersion

```c
const char* QAppManifestGetVersion(QAppManifestHandle hm, char* version, size_t max);
```

获取应用版本号字符串。

#### QAppManifestGetVersionCode

```c
int QAppManifestGetVersionCode(QAppManifestHandle hm);
```

获取应用版本号（整数）。

#### QAppManifestGetLogLevel

```c
const char* QAppManifestGetLogLevel(QAppManifestHandle hm, char* log_level, size_t max);
```

获取应用配置的日志级别。

#### QAppManifestGetDesignWidth

```c
int QAppManifestGetDesignWidth(QAppManifestHandle hm);
```

获取应用设计宽度（用于响应式布局计算）。

#### QAppManifestGetToolkitVersion

```c
const char* QAppManifestGetToolkitVersion(QAppManifestHandle hm, char* version, size_t max);
```

获取应用使用的 Toolkit 版本。

#### QAppManifestGetTextColor / QAppManifestGetBackgroundColor

```c
const char* QAppManifestGetTextColor(QAppManifestHandle hm, char* color, size_t max);
const char* QAppManifestGetBackgroundColor(QAppManifestHandle hm, char* color, size_t max);
```

获取应用默认文字颜色和背景颜色。

#### QAppManifestGetIconPath

```c
const char* QAppManifestGetIconPath(QAppManifestHandle hm, char* path, size_t max);
```

获取应用图标路径。

#### QAppManifestGetEntry

```c
const char* QAppManifestGetEntry(QAppManifestHandle hm, char* entry, size_t max);
```

获取应用入口文件路径。

### JS 环境接口

#### QApplicationGetJSEnv

```c
QApplicationJSEnvHandle QApplicationGetJSEnv(QApplicationHandle app);
```

获取应用的 JS 环境句柄。

#### QAppJSEnvGetRuntime

```c
QAppJSRuntimeHandle QAppJSEnvGetRuntime(QApplicationJSEnvHandle env);
```

获取 JS Runtime 句柄。

#### QAppJSEnvGetContext

```c
QAppJSContextHandle QAppJSEnvGetContext(QApplicationJSEnvHandle env);
```

获取 JS Context 句柄。

### GUI 相关接口

#### QApplicationGetGUIRoot

```c
NativeWidgetHandle QApplicationGetGUIRoot(QApplicationHandle app);
```

获取应用的 GUI 根控件句柄。

#### QApplicationPostUITask

```c
void QApplicationPostUITask(QApplicationHandle handle,
    void (*task_cb_onui)(void* user_data),
    void (*free_userdata)(void* user_data),
    void* user_data);
```

向 UI 线程投递异步任务。

- `handle`：应用句柄。
- `task_cb_onui`：在 UI 线程执行的回调。
- `free_userdata`：用户数据释放回调（可为 `NULL`）。
- `user_data`：用户自定义数据。

### 版本信息接口

#### QAppVersion

```c
const char* QAppVersion(void);
```

获取 QuickApp 框架版本号字符串。

#### QAppVersionCode

```c
int QAppVersionCode(void);
```

获取 QuickApp 框架版本号（整数）。

#### QAppAPILevel

```c
int QAppAPILevel(void);
```

获取 QuickApp 框架 API Level。

---

## gui_widget_context.h — GUI 组件上下文

提供 GUI 组件系统的初始化、事件循环绑定和组件上下文管理接口。

### 结构体

#### GUIWidgetContextCreateInfo

GUI 组件上下文创建参数。

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `gui_root` | `NativeWidgetHandle` | Native Widget 根控件入口 |
| `ui_message_loop` | `MessageLoop*` | UI 消息循环入口 |
| `package_name` | `const char*` | 应用包名 |
| `design_width` | `int` | 设计宽度 |
| `design_height` | `int` | 设计高度 |
| `async_mode` | `bool` | 是否启用异步模式 |

### 接口函数

#### GUIWidgetInit

```c
void GUIWidgetInit(void);
```

初始化 GUI 组件系统。应在应用启动时调用一次。

#### GUIWidgetUninit

```c
void GUIWidgetUninit(void);
```

反初始化 GUI 组件系统，释放全局资源。

#### GUILoopStart

```c
GuiDataHandle GUILoopStart(uv_loop_t* loop);
```

将 GUI 事件循环绑定到 libuv 事件循环。

- `loop`：libuv 事件循环指针。
- 返回：GUI 数据句柄。

#### GUILoopStop

```c
void GUILoopStop(GuiDataHandle gui_data);
```

停止 GUI 事件循环。

- `gui_data`：`GUILoopStart` 返回的句柄。

#### GUIWidgetContextCreate

```c
GUIWidgetContextHandle GUIWidgetContextCreate(const GUIWidgetContextCreateInfo* info);
```

创建 GUI 组件上下文。每个应用实例对应一个上下文。

- `info`：创建参数。
- 返回：上下文句柄。

#### GUIWidgetContextGetNativeHandle

```c
void* GUIWidgetContextGetNativeHandle(GUIWidgetContextHandle handle);
```

获取上下文关联的底层 Native 句柄。

- `handle`：上下文句柄。
- 返回：Native 句柄指针。

#### GUIWidgetContextGetScreen

```c
void* GUIWidgetContextGetScreen(GUIWidgetContextHandle handle);
```

获取上下文关联的屏幕对象。

- `handle`：上下文句柄。
- 返回：屏幕对象指针。

#### GUIWidgetContextDestroy

```c
void GUIWidgetContextDestroy(GUIWidgetContextHandle handle);
```

销毁 GUI 组件上下文，释放关联资源。

- `handle`：上下文句柄。

#### GUIWidgetContextCanIUse

```c
bool GUIWidgetContextCanIUse(const char* prop);
```

查询 GUI 组件系统是否支持指定属性或能力。

- `prop`：属性名称。
- 返回：`true` 表示支持。

---

## qwrapper.h — 平台适配接口

定义了需要由集成方（开发者）实现的平台适配接口。框架内部会调用这些函数获取平台相关的路径信息。

### 接口函数

#### QWrapperGetAppPath

```c
const char* QWrapperGetAppPath(char* buf, size_t max);
```

获取应用包的存储路径。开发者需根据目标平台的文件系统布局实现此接口。

- `buf`：输出缓冲区。
- `max`：缓冲区最大长度。
- 返回：路径字符串（即 `buf` 指针）。

#### QWrapperGetBaseDataDir

```c
const char* QWrapperGetBaseDataDir(const char* package_name, char* buf, size_t max);
```

获取指定应用的数据根目录。

- `package_name`：应用包名。
- `buf`：输出缓冲区。
- `max`：缓冲区最大长度。
- 返回：目录路径字符串。

---

## quickapp_inspector.h — Inspector 调试接口

提供基于 CDP（Chrome DevTools Protocol）的远程调试能力，包括网络请求拦截和存储监控。

> **说明：** 使用 Inspector 功能需要启用 `CONFIG_QUICKAPP_DEBUG` 配置项，并依赖 MQTT 网络通道。

### 接口函数

#### InspectStartServer

```c
void InspectStartServer();
```

启动 Inspector 调试服务。

#### InspectStopServer

```c
void InspectStopServer();
```

停止 Inspector 调试服务。

#### InspectHostNetRequest

```c
void InspectHostNetRequest(void* req, const char* method, int64_t request_id);
```

上报网络请求事件到 Inspector。

- `req`：请求对象指针。
- `method`：HTTP 方法（如 `"GET"`、`"POST"`）。
- `request_id`：请求 ID。

#### InspectHostNetResponse

```c
void InspectHostNetResponse(void* req, void* response, int64_t request_id, const char* header);
```

上报网络响应事件到 Inspector。

- `req`：请求对象指针。
- `response`：响应对象指针。
- `request_id`：请求 ID。
- `header`：响应头字符串。

#### InspectHostNetLoadingFailed

```c
void InspectHostNetLoadingFailed(bool ret);
```

上报网络加载失败事件。

- `ret`：是否为最终失败。

#### InspectHostNetGetReqId

```c
int64_t InspectHostNetGetReqId();
```

生成并获取一个新的网络请求 ID。

- 返回：新的请求 ID。

#### InspectHostNetGetCurrentReqId

```c
int64_t InspectHostNetGetCurrentReqId();
```

获取当前网络请求 ID（不递增）。

- 返回：当前请求 ID。

#### InspectHostStorageInit

```c
void InspectHostStorageInit(void* db);
```

初始化 Inspector 的存储监控。

- `db`：数据库实例指针。

#### InspectHostStorageUpdate

```c
void InspectHostStorageUpdate();
```

通知 Inspector 存储数据已更新。

---

## inspector_dump.h — 内存与性能诊断

提供应用级别的堆快照、内存分析和 CPU 性能分析接口，用于开发阶段的性能调优。

### 接口函数

#### InspectorDumpAppHeap

```c
void InspectorDumpAppHeap(QApplicationHandle app, const char* name);
```

生成应用的 JS 堆快照。

- `app`：应用句柄。
- `name`：快照名称标识。

#### InspectorDumpJSMem

```c
void InspectorDumpJSMem(QApplicationHandle app);
```

输出应用的 JS 内存使用统计信息。

- `app`：应用句柄。

#### InspectorStartAppHeapTimeline

```c
void InspectorStartAppHeapTimeline(QApplicationHandle app, const char* name);
```

开始记录堆内存时间线，用于追踪内存分配趋势。

- `app`：应用句柄。
- `name`：时间线名称标识。

#### InspectorStopAppHeapTimeline

```c
void InspectorStopAppHeapTimeline(QApplicationHandle app);
```

停止堆内存时间线记录。

- `app`：应用句柄。

#### InspectorStartAppCPUProfiling

```c
void InspectorStartAppCPUProfiling(QApplicationHandle app, const char* name);
```

开始 CPU 性能分析。

- `app`：应用句柄。
- `name`：分析会话名称标识。

#### InspectorStopAppCPUProfiling

```c
void InspectorStopAppCPUProfiling(QApplicationHandle app);
```

停止 CPU 性能分析并输出结果。

- `app`：应用句柄。
