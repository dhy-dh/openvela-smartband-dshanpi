# QuickApp API Reference

This document provides detailed descriptions of the types and functions exposed by the public header files under `include/`.

---

## Table of Contents

- [quickapp.h — Core Application API](#quickapph--core-application-api)
- [gui_widget_context.h — GUI Widget Context](#gui_widget_contexth--gui-widget-context)
- [qwrapper.h — Platform Adaptation Interface](#qwrapperh--platform-adaptation-interface)
- [quickapp_inspector.h — Inspector Debugging Interface](#quickapp_inspectorh--inspector-debugging-interface)
- [inspector_dump.h — Memory and Performance Diagnostics](#inspector_dumph--memory-and-performance-diagnostics)

---

## quickapp.h — Core Application API

The core header file of the QuickApp framework, providing interfaces for application creation, lifecycle management, page routing, manifest queries, and runtime status retrieval.

### Enumerations

#### QAppAnimationFunction

Page transition animation types.

| Value | Description |
| --- | --- |
| `QAPP_ANIMATION_DEFAUT` | Default animation |
| `QAPP_ANIMATION_NONE` | No animation |
| `QAPP_ANIMATION_SLIDE` | Slide animation |

#### QAppPageAction

Page action types, used as indices for the `QAppRouteInfo.page_animation` array.

| Value | Description |
| --- | --- |
| `QAPP_PAGE_OPEN_ENTER` | Enter animation when opening a page |
| `QAPP_PAGE_CLOSE_ENTER` | Enter animation when closing a page |
| `QAPP_PAGE_OPEN_EXIT` | Exit animation when opening a page |
| `QAPP_PAGE_CLOSE_EXIT` | Exit animation when closing a page |

#### QAPP_EVENT Event Codes

Framework event codes, delivered via the `QApplicationClient.OnAppNotifyEvent` callback.

| Value | Description |
| --- | --- |
| `QAPP_EVENT_START` | Application started |
| `QAPP_EVENT_BACKGROUND` | Application entered background |
| `QAPP_EVENT_EXIT` | Application exited; caller may release resources |
| `QAPP_EVENT_REQ_EXIT` | Application requests exit; caller should close the app |
| `QAPP_EVENT_OOM` | Out of memory |
| `QAPP_EVENT_ENOSPC` | No space left on device |
| `QAPP_EVENT_PREPARE` | Application preparing |
| `QAPP_EVENT_CREATE` | Application created |
| `QAPP_EVENT_SHOW` | Application shown |
| `QAPP_EVENT_HIDE` | Application hidden |
| `QAPP_EVENT_DESTROY` | Application destroyed |
| `QAPP_EVENT_PAGE_BACKPRESS` | Page back key event |
| `QAPP_EVENT_FIRST_PAGE_SHOW` | First page shown (can be used for launch animation control) |

#### QAppStatus

Application runtime status.

| Value | Description |
| --- | --- |
| `QAPP_STATUS_UNKWON` | Unknown status |
| `QAPP_STATUS_FOREGROUND` | Running in foreground |
| `QAPP_STATUS_SYSTEM_HOME` | System home |
| `QAPP_STATUS_BACKGROUND` | Running in background |

#### QAppPageLaunchMode

Page launch modes.

| Value | Description |
| --- | --- |
| `QAPP_PAGE_LAUCH_STANDARD` | Standard mode, creates a new instance each time |
| `QAPP_PAGE_LAUCH_SINGLETASK` | Single-task mode, reuses existing instance |

### Structures

#### QAppRouteInfo

Page routing information.

| Field | Type | Description |
| --- | --- | --- |
| `uri` | `const char*` | Route address, format: `schema://<package>/<pagepath>?<querystring>` |
| `querystring` | `const char*` | Query parameter string |
| `page_animation` | `int[QAPP_PAGE_ACTION_MAX]` | Animation type for each page action (see `QAppAnimationFunction`) |
| `clear_task` | `bool` | Whether to clear the task stack |
| `extra_data` | `const char*` | Extra data in JSON format |

#### QApplicationClient

Application client callback interface, implemented by the integrator and passed into `QApplicationCreateInfo`.

| Field | Type | Description |
| --- | --- | --- |
| `client_context` | `void*` | Client instance pointer |
| `OnAppNotifyEvent` | Function pointer | Application event notification callback; `qapp_event` values refer to the event code enumeration |
| `OnNavigateToApp` | Function pointer | Cross-application navigation callback; return `true` if handled |
| `OnAppDestroyOnUI` | Function pointer | Callback when the application is destroyed on the UI thread |
| `OnFeatureManagerPrepare` | Function pointer | Callback when Feature Manager is ready |
| `OnFeatureManagerDestroy` | Function pointer | Callback when Feature Manager is destroyed |

#### QApplicationCreateInfo

Application creation parameters.

| Field | Type | Description |
| --- | --- | --- |
| `uri` | `const char*` | Application URL |
| `client` | `QApplicationClient*` | Client callback interface |
| `gui_context` | `GUIWidgetContextHandle` | GUI widget context handle |
| `message_loop` | `MessageLoop*` | Application message loop |
| `ui_message_loop` | `MessageLoop*` | UI message loop |
| `stack_size` | `size_t` | JS stack size |
| `heap_size` | `size_t` | JS heap size |
| `entry` | `const char*` | Entry file path |
| `feature_manager_set` | Function pointer | Feature Manager setup callback |
| `feature_register` | Function pointer | Feature registration callback |
| `user_data` | `void*` | User-defined data |

#### QAppRunInfo

Application runtime information.

| Field | Type | Description |
| --- | --- | --- |
| `process_status` | `int` | Process status |
| `memory_useage_bytes` | `uint64_t` | Memory usage in bytes |
| `run_time_ms` | `uint64_t` | Running time in milliseconds |
| `hide_time_ms` | `uint64_t` | Background time in milliseconds |
| `priority` | `int` | Application priority |

#### QAppPageInfo

Page stack information.

| Field | Type | Description |
| --- | --- | --- |
| `name` | `const char*` | Page name |
| `path` | `const char*` | Page path |
| `lanch_mode` | `int` | Launch mode (see `QAppPageLaunchMode`) |

### Application Lifecycle Interfaces

#### QApplicationCreate

```c
QApplicationHandle QApplicationCreate(const QApplicationCreateInfo* info);
```

Creates a QuickApp instance.

- `info`: Application creation parameters. See `QApplicationCreateInfo`.
- Returns: Application handle, or `NULL` on failure.

#### QApplicationStart

```c
int QApplicationStart(QApplicationHandle app);
```

Starts the application, executing the JS entry file and entering the message loop.

- `app`: Application handle.
- Returns: 0 on success, non-zero on failure.

#### QApplicationRunOnce

```c
int QApplicationRunOnce(QApplicationHandle app);
```

Executes a single iteration of the application message loop. Useful when external control of the loop cadence is needed.

- `app`: Application handle.
- Returns: 0 on success.

#### QApplicationRequestExit

```c
void QApplicationRequestExit(QApplicationHandle app);
```

Called by the external caller (e.g., AMS or other integrators) to request the application to exit. This is the unified entry point for application exit — regardless of the reason, all exits should ultimately go through this interface.

- `app`: Application handle.

#### QApplicationSendExitEvent

```c
void QApplicationSendExitEvent(QApplicationHandle app);
```

Called internally by the application to notify the external caller of an exit event (`QAPP_EVENT_EXIT`). Upon receiving this event, the caller should invoke `QApplicationRequestExit` to complete the exit, ensuring consistency in the exit flow.

- `app`: Application handle.

> **Note:** There are two application exit scenarios:
> 1. External exit: The caller directly invokes `QApplicationRequestExit`.
> 2. Internal exit: The application first calls `QApplicationSendExitEvent` to notify the external caller, which then calls `QApplicationRequestExit` to complete the exit.
>
> This design ensures all exit paths go through the external caller for unified resource management and state synchronization.

#### QApplicationShow

```c
void QApplicationShow(QApplicationHandle app);
```

Switches the application to the foreground, triggering the `QAPP_EVENT_SHOW` event.

- `app`: Application handle.

#### QApplicationHide

```c
void QApplicationHide(QApplicationHandle app);
```

Switches the application to the background, triggering the `QAPP_EVENT_HIDE` event.

- `app`: Application handle.

#### QApplicationBackPress

```c
void QApplicationBackPress(QApplicationHandle app);
```

Simulates a back key press, triggering the `QAPP_EVENT_PAGE_BACKPRESS` event.

- `app`: Application handle.

#### QApplicationRunGC

```c
void QApplicationRunGC(QApplicationHandle app);
```

Manually triggers garbage collection in the JavaScript engine.

- `app`: Application handle.

#### QApplicationGetClient

```c
QApplicationClient* QApplicationGetClient(QApplicationHandle app);
```

Gets the client callback interface associated with the application.

- `app`: Application handle.
- Returns: Pointer to the client callback structure.

### Page Routing Interfaces

#### QApplicationRoute

```c
int QApplicationRoute(QApplicationHandle app, const char* uri);
```

Navigates to a page by URI.

- `app`: Application handle.
- `uri`: Target page URI.
- Returns: 0 on success.

#### QApplicationStackPagePush

```c
bool QApplicationStackPagePush(QApplicationHandle app, const QAppRouteInfo* route_info);
```

Pushes a new page onto the navigation stack.

- `app`: Application handle.
- `route_info`: Routing information.
- Returns: `true` on success.

#### QApplicationStackPageBack

```c
bool QApplicationStackPageBack(QApplicationHandle app, const char* back_path);
```

Navigates back. If `back_path` is specified, navigates back to the page matching that path.

- `app`: Application handle.
- `back_path`: Target page path, or `NULL` to go back one page.
- Returns: `true` on success.

#### QApplicationStackPageReplace

```c
bool QApplicationStackPageReplace(QApplicationHandle app, const QAppRouteInfo* route_info);
```

Replaces the current top page on the stack.

- `app`: Application handle.
- `route_info`: Routing information.
- Returns: `true` on success.

#### QApplicationClearStackPages

```c
bool QApplicationClearStackPages(QApplicationHandle app);
```

Clears the entire page navigation stack.

- `app`: Application handle.
- Returns: `true` on success.

#### QApplicationGetStackPageLength

```c
size_t QApplicationGetStackPageLength(QApplicationHandle app);
```

Gets the current depth of the page stack.

- `app`: Application handle.
- Returns: Number of pages in the stack.

#### QApplicationEnumStackPages

```c
void QApplicationEnumStackPages(QApplicationHandle app,
    bool (*enum_cb)(int stack_index, const QAppPageInfo* page_info, void* user_data),
    void* user_data);
```

Enumerates pages in the stack, starting from the top. Returning `false` from the callback terminates the enumeration early.

- `app`: Application handle.
- `enum_cb`: Enumeration callback function.
- `user_data`: User-defined data passed through to the callback.

### Application Information Interfaces

#### QApplicationGetPackageName

```c
const char* QApplicationGetPackageName(QApplicationHandle app);
```

Gets the application package name.

- `app`: Application handle.
- Returns: Package name string.

#### QApplicationGetPackagePath

```c
const char* QApplicationGetPackagePath(QApplicationHandle app, char* path, size_t max);
```

Gets the application package path.

- `app`: Application handle.
- `path`: Output buffer.
- `max`: Maximum buffer length.
- Returns: Path string.

#### QApplicationGetTempDir / QApplicationGetCacheDir / QApplicationGetDataDir

```c
const char* QApplicationGetTempDir(QApplicationHandle app, char* path, size_t max);
const char* QApplicationGetCacheDir(QApplicationHandle app, char* path, size_t max);
const char* QApplicationGetDataDir(QApplicationHandle app, char* path, size_t max);
```

Gets the application's temporary, cache, and data directory paths respectively.

- `app`: Application handle.
- `path`: Output buffer.
- `max`: Maximum buffer length.
- Returns: Directory path string.

#### QApplicationGetRunInfo

```c
bool QApplicationGetRunInfo(QApplicationHandle app, QAppRunInfo* pinfo);
```

Gets application runtime information (memory usage, running time, etc.).

- `app`: Application handle.
- `pinfo`: Output parameter. See `QAppRunInfo`.
- Returns: `true` on success.

#### QApplicationHasFeature

```c
bool QApplicationHasFeature(QApplicationHandle app, const char* feature);
```

Checks whether the application has the specified Feature capability.

- `app`: Application handle.
- `feature`: Feature name.
- Returns: `true` if the feature is available.

#### QApplicationIsDeclearedFeature

```c
bool QApplicationIsDeclearedFeature(QApplicationHandle app, const char* feature);
```

Checks whether the specified Feature is declared in the application manifest.

- `app`: Application handle.
- `feature`: Feature name.
- Returns: `true` if declared.

#### QApplicationGetFeatureManager

```c
FeatureManagerHandle QApplicationGetFeatureManager(QApplicationHandle app);
```

Gets the application's Feature Manager handle.

- `app`: Application handle.
- Returns: Feature Manager handle.

### Manifest Query Interfaces

#### QApplicationGetManifest

```c
QAppManifestHandle QApplicationGetManifest(QApplicationHandle app);
```

Gets the application manifest handle.

- `app`: Application handle.
- Returns: Manifest handle.

#### QAppManifestGetAppName

```c
const char* QAppManifestGetAppName(QAppManifestHandle hm, char* app_name, size_t max);
```

Gets the application name.

#### QAppManifestGetVersion

```c
const char* QAppManifestGetVersion(QAppManifestHandle hm, char* version, size_t max);
```

Gets the application version string.

#### QAppManifestGetVersionCode

```c
int QAppManifestGetVersionCode(QAppManifestHandle hm);
```

Gets the application version code (integer).

#### QAppManifestGetLogLevel

```c
const char* QAppManifestGetLogLevel(QAppManifestHandle hm, char* log_level, size_t max);
```

Gets the application's configured log level.

#### QAppManifestGetDesignWidth

```c
int QAppManifestGetDesignWidth(QAppManifestHandle hm);
```

Gets the application design width (used for responsive layout calculation).

#### QAppManifestGetToolkitVersion

```c
const char* QAppManifestGetToolkitVersion(QAppManifestHandle hm, char* version, size_t max);
```

Gets the toolkit version used by the application.

#### QAppManifestGetTextColor / QAppManifestGetBackgroundColor

```c
const char* QAppManifestGetTextColor(QAppManifestHandle hm, char* color, size_t max);
const char* QAppManifestGetBackgroundColor(QAppManifestHandle hm, char* color, size_t max);
```

Gets the application's default text color and background color.

#### QAppManifestGetIconPath

```c
const char* QAppManifestGetIconPath(QAppManifestHandle hm, char* path, size_t max);
```

Gets the application icon path.

#### QAppManifestGetEntry

```c
const char* QAppManifestGetEntry(QAppManifestHandle hm, char* entry, size_t max);
```

Gets the application entry file path.

### JS Environment Interfaces

#### QApplicationGetJSEnv

```c
QApplicationJSEnvHandle QApplicationGetJSEnv(QApplicationHandle app);
```

Gets the application's JS environment handle.

#### QAppJSEnvGetRuntime

```c
QAppJSRuntimeHandle QAppJSEnvGetRuntime(QApplicationJSEnvHandle env);
```

Gets the JS Runtime handle.

#### QAppJSEnvGetContext

```c
QAppJSContextHandle QAppJSEnvGetContext(QApplicationJSEnvHandle env);
```

Gets the JS Context handle.

### GUI Interfaces

#### QApplicationGetGUIRoot

```c
NativeWidgetHandle QApplicationGetGUIRoot(QApplicationHandle app);
```

Gets the application's GUI root widget handle.

#### QApplicationPostUITask

```c
void QApplicationPostUITask(QApplicationHandle handle,
    void (*task_cb_onui)(void* user_data),
    void (*free_userdata)(void* user_data),
    void* user_data);
```

Posts an asynchronous task to the UI thread.

- `handle`: Application handle.
- `task_cb_onui`: Callback to execute on the UI thread.
- `free_userdata`: User data release callback (may be `NULL`).
- `user_data`: User-defined data.

### Version Information Interfaces

#### QAppVersion

```c
const char* QAppVersion(void);
```

Gets the QuickApp framework version string.

#### QAppVersionCode

```c
int QAppVersionCode(void);
```

Gets the QuickApp framework version code (integer).

#### QAppAPILevel

```c
int QAppAPILevel(void);
```

Gets the QuickApp framework API level.

---

## gui_widget_context.h — GUI Widget Context

Provides interfaces for GUI widget system initialization, event loop binding, and widget context management.

### Structures

#### GUIWidgetContextCreateInfo

GUI widget context creation parameters.

| Field | Type | Description |
| --- | --- | --- |
| `gui_root` | `NativeWidgetHandle` | Native widget root entry |
| `ui_message_loop` | `MessageLoop*` | UI message loop entry |
| `package_name` | `const char*` | Application package name |
| `design_width` | `int` | Design width |
| `design_height` | `int` | Design height |
| `async_mode` | `bool` | Whether to enable async mode |

### Functions

#### GUIWidgetInit

```c
void GUIWidgetInit(void);
```

Initializes the GUI widget system. Should be called once at application startup.

#### GUIWidgetUninit

```c
void GUIWidgetUninit(void);
```

Deinitializes the GUI widget system and releases global resources.

#### GUILoopStart

```c
GuiDataHandle GUILoopStart(uv_loop_t* loop);
```

Binds the GUI event loop to a libuv event loop.

- `loop`: libuv event loop pointer.
- Returns: GUI data handle.

#### GUILoopStop

```c
void GUILoopStop(GuiDataHandle gui_data);
```

Stops the GUI event loop.

- `gui_data`: Handle returned by `GUILoopStart`.

#### GUIWidgetContextCreate

```c
GUIWidgetContextHandle GUIWidgetContextCreate(const GUIWidgetContextCreateInfo* info);
```

Creates a GUI widget context. Each application instance corresponds to one context.

- `info`: Creation parameters.
- Returns: Context handle.

#### GUIWidgetContextGetNativeHandle

```c
void* GUIWidgetContextGetNativeHandle(GUIWidgetContextHandle handle);
```

Gets the underlying native handle associated with the context.

- `handle`: Context handle.
- Returns: Native handle pointer.

#### GUIWidgetContextGetScreen

```c
void* GUIWidgetContextGetScreen(GUIWidgetContextHandle handle);
```

Gets the screen object associated with the context.

- `handle`: Context handle.
- Returns: Screen object pointer.

#### GUIWidgetContextDestroy

```c
void GUIWidgetContextDestroy(GUIWidgetContextHandle handle);
```

Destroys the GUI widget context and releases associated resources.

- `handle`: Context handle.

#### GUIWidgetContextCanIUse

```c
bool GUIWidgetContextCanIUse(const char* prop);
```

Queries whether the GUI widget system supports a specified property or capability.

- `prop`: Property name.
- Returns: `true` if supported.

---

## qwrapper.h — Platform Adaptation Interface

Defines platform adaptation interfaces that must be implemented by the integrator (developer). The framework calls these functions internally to obtain platform-specific path information.

### Functions

#### QWrapperGetAppPath

```c
const char* QWrapperGetAppPath(char* buf, size_t max);
```

Gets the application package storage path. Developers must implement this based on the target platform's filesystem layout.

- `buf`: Output buffer.
- `max`: Maximum buffer length.
- Returns: Path string (i.e., the `buf` pointer).

#### QWrapperGetBaseDataDir

```c
const char* QWrapperGetBaseDataDir(const char* package_name, char* buf, size_t max);
```

Gets the data root directory for the specified application.

- `package_name`: Application package name.
- `buf`: Output buffer.
- `max`: Maximum buffer length.
- Returns: Directory path string.

---

## quickapp_inspector.h — Inspector Debugging Interface

Provides remote debugging capabilities based on CDP (Chrome DevTools Protocol), including network request interception and storage monitoring.

> **Note:** Using Inspector requires enabling the `CONFIG_QUICKAPP_DEBUG` configuration option and depends on an MQTT network channel.

### Functions

#### InspectStartServer

```c
void InspectStartServer();
```

Starts the Inspector debugging server.

#### InspectStopServer

```c
void InspectStopServer();
```

Stops the Inspector debugging server.

#### InspectHostNetRequest

```c
void InspectHostNetRequest(void* req, const char* method, int64_t request_id);
```

Reports a network request event to the Inspector.

- `req`: Request object pointer.
- `method`: HTTP method (e.g., `"GET"`, `"POST"`).
- `request_id`: Request ID.

#### InspectHostNetResponse

```c
void InspectHostNetResponse(void* req, void* response, int64_t request_id, const char* header);
```

Reports a network response event to the Inspector.

- `req`: Request object pointer.
- `response`: Response object pointer.
- `request_id`: Request ID.
- `header`: Response header string.

#### InspectHostNetLoadingFailed

```c
void InspectHostNetLoadingFailed(bool ret);
```

Reports a network loading failure event.

- `ret`: Whether this is a final failure.

#### InspectHostNetGetReqId

```c
int64_t InspectHostNetGetReqId();
```

Generates and returns a new network request ID.

- Returns: New request ID.

#### InspectHostNetGetCurrentReqId

```c
int64_t InspectHostNetGetCurrentReqId();
```

Gets the current network request ID (without incrementing).

- Returns: Current request ID.

#### InspectHostStorageInit

```c
void InspectHostStorageInit(void* db);
```

Initializes Inspector storage monitoring.

- `db`: Database instance pointer.

#### InspectHostStorageUpdate

```c
void InspectHostStorageUpdate();
```

Notifies the Inspector that storage data has been updated.

---

## inspector_dump.h — Memory and Performance Diagnostics

Provides application-level heap snapshots, memory analysis, and CPU profiling interfaces for performance tuning during development.

### Functions

#### InspectorDumpAppHeap

```c
void InspectorDumpAppHeap(QApplicationHandle app, const char* name);
```

Generates a JS heap snapshot for the application.

- `app`: Application handle.
- `name`: Snapshot name identifier.

#### InspectorDumpJSMem

```c
void InspectorDumpJSMem(QApplicationHandle app);
```

Outputs JS memory usage statistics for the application.

- `app`: Application handle.

#### InspectorStartAppHeapTimeline

```c
void InspectorStartAppHeapTimeline(QApplicationHandle app, const char* name);
```

Starts recording a heap memory timeline to track memory allocation trends.

- `app`: Application handle.
- `name`: Timeline name identifier.

#### InspectorStopAppHeapTimeline

```c
void InspectorStopAppHeapTimeline(QApplicationHandle app);
```

Stops heap memory timeline recording.

- `app`: Application handle.

#### InspectorStartAppCPUProfiling

```c
void InspectorStartAppCPUProfiling(QApplicationHandle app, const char* name);
```

Starts CPU profiling.

- `app`: Application handle.
- `name`: Profiling session name identifier.

#### InspectorStopAppCPUProfiling

```c
void InspectorStopAppCPUProfiling(QApplicationHandle app);
```

Stops CPU profiling and outputs the results.

- `app`: Application handle.
