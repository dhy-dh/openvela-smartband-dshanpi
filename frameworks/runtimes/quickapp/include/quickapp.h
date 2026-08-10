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

#ifndef QUICKAPP_H
#define QUICKAPP_H

#include <stdint.h>

#include "feature_main_exports.h"
#include "gui_widget_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MessageLoop MessageLoop;
typedef struct QApplication QApplication;
typedef struct QApplication* QApplicationHandle;

typedef enum {
    QAPP_ANIMATION_DEFAUT,
    QAPP_ANIMATION_NONE,
    QAPP_ANIMATION_SLIDE
} QAppAnimationFunction;

typedef enum {
    QAPP_PAGE_OPEN_ENTER,
    QAPP_PAGE_CLOSE_ENTER,
    QAPP_PAGE_OPEN_EXIT,
    QAPP_PAGE_CLOSE_EXIT,
    QAPP_PAGE_ACTION_MAX
} QAppPageAction;

typedef struct {
    const char* uri; // schema://<package>/<pagepath>?<queruystring>
    const char* querystring;
    int page_animation[QAPP_PAGE_ACTION_MAX];
    bool clear_task;
    const char* extra_data; // json format
} QAppRouteInfo;

// event code of quickapp framework.  Users can set listener
// function `NotifyEvent` in QApplicationClient to handle these events.
enum {
    QAPP_EVENT_BEGIN = 0,
    QAPP_EVENT_START = 1, // application start
    QAPP_EVENT_BACKGROUND = 2, // application background
    QAPP_EVENT_EXIT = 3, // application exit. Now, ams could release resource
    QAPP_EVENT_REQ_EXIT = 4, // application request exit. Responded by ams, do close app
    QAPP_EVENT_OOM = 5, // out of memory
    QAPP_EVENT_ENOSPC = 6, // no space left on device

    QAPP_EVENT_PREPARE,
    QAPP_EVENT_CREATE,
    QAPP_EVENT_SHOW,
    QAPP_EVENT_HIDE,
    QAPP_EVENT_DESTROY,
    QAPP_EVENT_PAGE_BACKPRESS,
    QAPP_EVENT_FIRST_PAGE_SHOW, // use this event to show splash

    /*QAPP_EVENT_PAGE_INT,
    QAPP_EVENT_PAGE_READY,
    QAPP_EVENT_PAGE_SHOW,
    QAPP_EVENT_PAGE_HIDE,
    QAPP_EVENT_PAGE_DESTROY,
    QAPP_EVENT_PAGE_REFRESH,
    QAPP_EVENT_PAGE_CONFIGUARTION_CHANGED,
    */
    QAPP_EVENT_END,
};

typedef struct QApplicationClient {
    void* client_context; // Client instance
    void (*OnAppNotifyEvent)(struct QApplicationClient* app_client, int qapp_event, void* params);
    bool (*OnNavigateToApp)(struct QApplicationClient* app_client_, const QAppRouteInfo* info);
    void (*OnAppDestroyOnUI)(struct QApplicationClient* app_client);
    void (*OnFeatureManagerPrepare)(struct QApplicationClient* app_client);
    void (*OnFeatureManagerDestroy)(struct QApplicationClient* app_client);
} QApplicationClient;

typedef QApplicationClient* QApplicationClientHandle;

typedef struct QApplicationCreateInfo {
    const char* uri; // application url
    QApplicationClient* client;
    GUIWidgetContextHandle gui_context;
    MessageLoop* message_loop; // application的message_loop
    MessageLoop* ui_message_loop;
    // 初始化专用接口
    size_t stack_size;
    size_t heap_size;
    const char* entry; // 入口文件
    // Feature Manager user data 设置
    int (*feature_manager_set)(QApplicationHandle handle, FeatureManagerHandle feature_manager, void* user_data);
    // Feature注册文件
    int (*feature_register)(QApplicationHandle handle, FeatureRegistryHandle h_registry, void* user_data);
    void* user_data;
} QApplicationCreateInfo;

QApplicationHandle QApplicationCreate(const QApplicationCreateInfo* info);

/**
 * @brief Request application exit, call by ams
 */
void QApplicationRequestExit(QApplicationHandle app);

/**
 * @brief send exit event to application
 */
void QApplicationSendExitEvent(QApplicationHandle app);

// 获取接口
QApplicationClient* QApplicationGetClient(QApplicationHandle app);

// 开启运行
int QApplicationStart(QApplicationHandle app);

int QApplicationRunOnce(QApplicationHandle app);

void QApplicationBackPress(QApplicationHandle app);

int QApplicationRoute(QApplicationHandle app, const char* uri);

void QApplicationShow(QApplicationHandle app);
void QApplicationHide(QApplicationHandle app);

void QApplicationRunGC(QApplicationHandle app);

typedef struct _QApplicationJSEnv* QApplicationJSEnvHandle;
typedef struct _QAppJSRuntime* QAppJSRuntimeHandle;
typedef struct _QAppJSContext* QAppJSContextHandle;

QApplicationJSEnvHandle QApplicationGetJSEnv(QApplicationHandle app);
QAppJSRuntimeHandle QAppJSEnvGetRuntime(QApplicationJSEnvHandle env);
QAppJSContextHandle QAppJSEnvGetContext(QApplicationJSEnvHandle env);

bool QApplicationHasFeature(QApplicationHandle app, const char* feature);

bool QApplicationIsDeclearedFeature(QApplicationHandle app, const char* feature);

const char* QApplicationGetPackageName(QApplicationHandle app);

const char* QApplicationGetPackagePath(QApplicationHandle app, char* path, size_t max);

const char* QApplicationGetTempDir(QApplicationHandle app, char* path, size_t max);
const char* QApplicationGetCacheDir(QApplicationHandle app, char* path, size_t max);
const char* QApplicationGetDataDir(QApplicationHandle app, char* path, size_t max);

NativeWidgetHandle QApplicationGetGUIRoot(QApplicationHandle app);
void QApplicationPostUITask(QApplicationHandle handle, void (*task_cb_onui)(void* user_data),
    void (*free_userdata)(void* user_data), void* user_data);

//////////////////////////////////////////////
// Interface of AppManifest
typedef struct _QApplicationManifest* QAppManifestHandle;

QAppManifestHandle QApplicationGetManifest(QApplicationHandle app);

const char* QAppManifestGetAppName(QAppManifestHandle hm, char* app_name, size_t max);
const char* QAppManifestGetVersion(QAppManifestHandle hm, char* version, size_t max);
int QAppManifestGetVersionCode(QAppManifestHandle hm);
const char* QAppManifestGetLogLevel(QAppManifestHandle hm, char* log_level, size_t max);
int QAppManifestGetDesignWidth(QAppManifestHandle hm);
const char* QAppManifestGetToolkitVersion(QAppManifestHandle hm, char* version, size_t max);
const char* QAppManifestGetTextColor(QAppManifestHandle hm, char* color, size_t max);
const char* QAppManifestGetBackgroundColor(QAppManifestHandle hm, char* color, size_t max);
const char* QAppManifestGetIconPath(QAppManifestHandle hm, char* path, size_t max);
const char* QAppManifestGetEntry(QAppManifestHandle hm, char* entry, size_t max);

typedef enum {
    QAPP_STATUS_UNKWON,
    QAPP_STATUS_FOREGROUND,
    QAPP_STATUS_SYSTEM_HOME,
    QAPP_STATUS_BACKGROUND,
} QAppStatus;

typedef enum {
    QAPP_PRIORITY_PERSISTENT = 100,
    QAPP_PRIORITY_HIGH = 200,
    QAPP_PRIORITY_MIDDLE = 300,
    QAPP_PRIORITY_LOW = 400,
    QAPP_PRIORITY_ERROR = -1
} QAppPriority;

typedef struct QAppRunInfo {
    int process_status;
    uint64_t memory_useage_bytes;
    uint64_t run_time_ms;
    uint64_t hide_time_ms;
    int priority;
} QAppRunInfo;

bool QApplicationGetRunInfo(QApplicationHandle app, QAppRunInfo* pinfo);

FeatureManagerHandle QApplicationGetFeatureManager(QApplicationHandle app);

typedef enum {
    QAPP_PAGE_LAUCH_STANDARD,
    QAPP_PAGE_LAUCH_SINGLETASK
} QAppPageLaunchMode;

// functions for nativagor
typedef struct QAppPageInfo {
    const char* name;
    const char* path;
    int lanch_mode;
} QAppPageInfo;

// top page will enum fisrtly
//
void QApplicationEnumStackPages(QApplicationHandle app,
    bool (*enum_cb)(int stack_index, const QAppPageInfo* page_info, void* user_data),
    void* user_data);

size_t QApplicationGetStackPageLength(QApplicationHandle app);
bool QApplicationClearStackPages(QApplicationHandle app);
bool QApplicationStackPageBack(QApplicationHandle app, const char* back_path);
bool QApplicationStackPageReplace(QApplicationHandle app, const QAppRouteInfo* route_info);
bool QApplicationStackPagePush(QApplicationHandle app, const QAppRouteInfo* route_info);

/////////////////////////////////////////////////////////////
// install the package
typedef struct _QAppInfo {
    char package[128];
    char icon[64];
    char versionName[64];
    int versionCode;
    unsigned int minAPILevel;
    int minPlatformVersion;
    unsigned char fingerprint[20];
    int fingerprint_len;
    bool standalone; // APP是否依赖手机 默认false
    bool needNetwork; // APP是否依赖网络 默认false
    bool background; // APP是否支持后台运行
    uint64_t installedTimestamp; // install timestamp
    int name_len;
    struct i18NEntry {
        char lang[16]; // zh-CN, en ... etc
        char value[64]; // 具体值
    } * names;
    FeaturePermissions permissions;
} QAppInfo;

/**
 * @brief Compare two QAppInfo struct instances for complete equality
 * @param info1 Pointer to the first QAppInfo instance (NULL allowed)
 * @param info2 Pointer to the second QAppInfo instance (NULL allowed)
 * @return 1 if all fields of the two instances are identical, 0 otherwise
 */
int QAppInfoEqual(QAppInfo* info1, QAppInfo* info2);

void QAppInfoFree(QAppInfo* info);

typedef struct _QAppInstallParam {
    int stage;
    const char* rpkfile;
    const QAppInfo* app_info;
    const char* icon_path;
    void* user_data;
    union {
        float progress;
        struct {
            int code;
            const char* message;
        } error;
    };
} QAppInstallParam;

typedef enum {
    QAPP_INSTALL_PREPARE,
    QAPP_INSTALL_RUNNING,
    QAPP_INSTALL_FINISHED,
    QAPP_INSTALL_CANCELED,
    QAPP_INSTALL_ERROR
} QAppInstallStage;

typedef struct _QAppInstallTask* QAppInstallTaskHandle;
/**
 * @brief QAppInstallCallback
 * @param param install param. The memory of param is managed by quickapp framework,
 *              The content of param is saved in install task internal structure,
 *              so do not free it in callback function. if you want to save the data,
 *              please copy it or keep the install task handle until you finish using the data.
 */
typedef void (*QAppInstallCallback)(const QAppInstallParam*);
QAppInstallTaskHandle QApplicationInstallRpk(const char* rpk_file,
    QAppInstallCallback install_cb,
    void* user_data);

bool QAppInstallTaskCancel(QAppInstallTaskHandle handle);
int QAppInstallTaskGetStage(QAppInstallTaskHandle handle);
void QAppInstallTaskDestroy(QAppInstallTaskHandle handle);

typedef struct _QAppUninstallParam {
    int stage;
    const char* package_path;
    void* user_data;
    union {
        float progress;
        struct {
            int code;
            const char* message;
        } error;
    };
} QAppUninstallParam;

typedef enum {
    QAPP_UNINSTALL_PREPARE,
    QAPP_UNINSTALL_RUNNING,
    QAPP_UNINSTALL_FINISHED,
    QAPP_UNINSTALL_CANCELED,
    QAPP_UNINSTALL_ERROR
} QAppUninstallStage;

typedef struct _QAppUninstallTask* QAppUninstallTaskHandle;

/**
 * @brief QAppUninstallCallback
 * @param param uninstall param. The memory of param is managed by quickapp framework,
 *              The content of param is saved in install task internal structure,
 *              so do not free it in callback function. if you want to save the data,
 *              please copy it or keep the install task handle until you finish using the data.
 */
typedef void (*QAppUninstallCallback)(const QAppUninstallParam*);

QAppUninstallTaskHandle QApplicationUninstallRpk(const char* rpk_file,
    QAppUninstallCallback uninstall_cb,
    void* user_data, bool fake_remove);
bool QAppUninstallTaskCancel(QAppUninstallTaskHandle handle);
int QAppUninstallTaskGetState(QAppUninstallTaskHandle handle);
void QAppUninstallTaskDestroy(QAppUninstallTaskHandle handle);

const char* QAppVersion(void);
int QAppVersionCode(void);
int QAppAPILevel(void);

#ifdef __cplusplus
}
#endif

#endif /* QUICKAPP_H */
