
#include "shell_app.h"

#include <filesystem>
#include <pthread.h>
#include <sys/prctl.h>

#include <ash/message_loop/cmessage_loop.h>

#include "common/shell_utils.h"
#include "feature_exports.h"
#ifdef CONFIG_FEATURE_RUST_MODULES
#include "internal/feature_rust.h"
#endif

#define HAP_PREFIX "hap://app/"

extern FeatureRegistryTableHandle g_ajs_features_registry;

namespace fs = std::filesystem;

/**
 * @brief 缺省大小
 */
const uint64_t kDefaultReadSize = 65536;

#if defined(AIOTJS_HAVE_SANITIZE) && !defined(__NuttX__)
const uint32_t kDefaultJSStackSize = 1024U * 1024U * 4U;
const uint32_t kDefaultThreadStackSize = 1024U * 1024U * 5U;
const uint32_t kDefaultJSHeapSize = 1024U * 1024U * 15U;
#elif defined(__NuttX__)
const uint32_t kDefaultJSStackSize = CONFIG_QUICKAPP_JSSTACKSIZE;
const uint32_t kDefaultThreadStackSize = CONFIG_QUICKAPP_THREADSTACKSIZE;
const uint32_t kDefaultJSHeapSize = CONFIG_QUICKAPP_JSHEAPSIZE;
#else
const uint32_t kDefaultJSStackSize = 512U * 1024U;
const uint32_t kDefaultThreadStackSize = 1024U * 1024U * 1U;
const uint32_t kDefaultJSHeapSize = 1024U * 1024U * 5U;
#endif

namespace shell {

ShellApp::ShellApp(const char* url, MessageLoop* ui_message_loop, std::unique_ptr<Delegate> delegate, bool async_mode)
    : async_mode_(async_mode)
    , owned_loop_(true)
    , quick_app_(nullptr)
    , gui_context_(nullptr)
    , ui_loop_(nullptr)
    , root_ui_(nullptr)
    , ui_message_loop_(ui_message_loop)
    , delegate_(std::move(delegate))
{
    initUrl(url);
}

std::string ShellApp::getIconPath()
{
    QAppManifestHandle hmanifest = QApplicationGetManifest(quick_app_);
    char package_buf[PATH_MAX], icon_buf[PATH_MAX];

    QApplicationGetPackagePath(quick_app_, package_buf, sizeof(package_buf));
    QAppManifestGetIconPath(hmanifest, icon_buf, sizeof(icon_buf));
    return (fs::path(package_buf) / fs::path(icon_buf).lexically_relative("/")).string();
}

void ShellApp::create(NativeWidgetHandle root)
{
    initAppClient();
    // 创建main_loop
    root_ui_ = delegate_->transformRootWidget(root);

    // 初始化 gui context
    // 无论哪种模式，onCreate函数始终和GUI在同一线程，所以这里直接调用
    initGUIContext(root_ui_, ui_message_loop_);
#ifdef CONFIG_QUICKAPP_PERMISSION_MANAGER
    permission_manager_ = std::move(PermissionManager::Create());
#endif
    // 初始化 Application
    if (async_mode_) {
        initApplicationAsync();
    } else {
        app_loop_ = ui_loop_; // 非异步模式，app_loop就是UI线程
        initApplication(ui_message_loop_);
        start();
    }
}

void ShellApp::initGUIContext(NativeWidgetHandle root, MessageLoop* ui_loop)
{
    // 无论哪种模式，
    GUIWidgetContextCreateInfo info;

    info.gui_root = root;
    info.ui_message_loop = ui_loop; // main_loop就是UI线程。

    info.package_name = package_name_.c_str();
    info.design_width = 480; // TODO
    info.design_height = 480; // TODO
    info.async_mode = async_mode_;

    // 创建context
    gui_context_ = GUIWidgetContextCreate(&info);
}

void ShellApp::initApplication(MessageLoop* message_loop)
{
    // 创建App, 需要在App所在线程创建
    QApplicationCreateInfo info = { 0 };

    std::string uri = url_;
    info.uri = uri.c_str();
    info.client = &app_client_; // 获取 app_client
    info.gui_context = gui_context_;
    info.message_loop = message_loop;
    info.stack_size = kDefaultThreadStackSize;
    info.heap_size = kDefaultJSHeapSize;
    info.entry = NULL; //设定入口
    info.ui_message_loop = ui_message_loop_;

    info.feature_register = [](QApplicationHandle handle, FeatureRegistryHandle hRegistry, void* user_data) -> int {
        auto* self = (ShellApp*)user_data;
        FeatureRegisterFeatures(hRegistry, g_ajs_features_registry);
        self->delegate_->initCustomFeatures(hRegistry);
        return 0;
    };

    info.feature_manager_set = [](QApplicationHandle handle, FeatureManagerHandle feature_manager, void* user_data) -> int {
        auto* self = (ShellApp*)user_data;
        for (auto& [key, value] : self->feature_manager_user_data_) {
            FeatureSetManagerUserData(feature_manager, key.c_str(), value);
        }
        return 0;
    };

    info.user_data = this;

    quick_app_ = QApplicationCreate(&info);

    if (permission_manager_) {
        permission_manager_->init(getPackageName(), QApplicationGetFeatureManager(getQuickApp()));
    }
}

struct ApplicationThreadStartInfo {
    ShellApp* self;
    uv_sem_t sem;
};

void ShellApp::initApplicationAsync()
{
    ApplicationThreadStartInfo info;
    info.self = this;
    uv_sem_init(&info.sem, 0);

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    attr.stacksize = kDefaultThreadStackSize;
    pthread_t thread_id;
    pthread_create(&thread_id, &attr,
        (pthread_startroutine_t)runApplication, &info);

    // 等待信号
    uv_sem_wait(&info.sem);
    uv_sem_destroy(&info.sem);
}
static void ensureUVLoopCleanup(uv_loop_t* loop)
{
    // 清除事件循环(尝试200次，总计4s)
    int closed = 0;
    for (int i = 0; i < 200; i++) {
        if (uv_loop_close(loop) == 0) {
            closed = 1;
            break;
        }
        // sleep 20 ms, wait for other threads to be finished
        usleep(20000);
        uv_run(loop, UV_RUN_NOWAIT);
    }
    if (!closed) {
        FILE* fp = fopen("/dev/log", "wb");
        fp = fp ? fp : stderr;
        uv_print_all_handles(loop, fp);
        if (fp != stderr) {
            fclose(fp);
        }
        // assert directly if we can't stop uv loop successfuly.
        SHELL_CHECK(false && "uv loop close timeout !");
    }
}

void ShellApp::runApplication(void* arg)
{
    ApplicationThreadStartInfo* info = (ApplicationThreadStartInfo*)arg;
    auto* self = info->self;

    // 设置线程名
    prctl(PR_SET_NAME, self->getPackageName().c_str());

    // 创建message loop
    uv_loop_t js_loop;
    uv_loop_init(&js_loop);
    MessageLoop* message_loop = MessageLoop_CreateForUV(&js_loop);
    self->app_loop_ = &js_loop;

    self->js_task_queue_ = std::move(MakeTaskQueueFromUV(&js_loop));

    self->initApplication(message_loop);

    self->start(); //启动application

    // 发出信号
    uv_sem_post(&info->sem);

    // 运行消息循环
    uv_run(&js_loop, UV_RUN_DEFAULT);

    // 关闭message_loop
    MessageLoop_Destroy(message_loop);

    ensureUVLoopCleanup(&js_loop);

    // 发出退出事件
    self->onNotifyEvent(QAPP_EVENT_EXIT, nullptr);
}

void ShellApp::start()
{
    QApplicationStart(quick_app_);
}

void ShellApp::stop()
{
    if (!quick_app_) {
        return;
    }

    if (permission_manager_) {
        permission_manager_->uninit();
        permission_manager_.reset();
    }

    QApplicationRequestExit(quick_app_);
    quick_app_ = nullptr;
}

uv_loop_t* ShellApp::getUILoop()
{
    if (ui_loop_ == nullptr) {
        if (owned_loop_) {
            ui_loop_ = new uv_loop_t;
            uv_loop_init(ui_loop_);
        }
    }

    return ui_loop_;
}

void ShellApp::runMainLoop()
{
    if (ui_loop_) {
        uv_run(ui_loop_, UV_RUN_DEFAULT);
    }
}

void ShellApp::waitUIDestroy()
{
    assert(ui_loop_);
    int loop_limits = 200;
    while (gui_context_ && loop_limits-- > 0) {
        usleep(20000);
        uv_run(ui_loop_, UV_RUN_NOWAIT);
    }

    SHELL_LOG_INFO("loop_limits left:%d\n", loop_limits);
    if (gui_context_) {
        SHELL_LOG_ERROR("ui destroy timeout");
        SHELL_CHECK(false && "gui_context_ release timeout!");
    }
}

void ShellApp::initUrl(const char* url)
{
    int pos = sizeof(HAP_PREFIX) - 1;
    const char* pkg_start = url;
    if (strncmp(url, HAP_PREFIX, pos) == 0) {
        pkg_start += pos;
    }

    const char* path = strchr(pkg_start, '/');
    int pkg_len = path ? path - pkg_start : strlen(pkg_start);
    package_name_.clear();
    package_name_.append(pkg_start, pkg_len);

    url_ = HAP_PREFIX;
    url_ += pkg_start;
}

struct SharedShellApp {
    std::shared_ptr<ShellApp> app;
};

void ShellApp::release()
{
    delete static_cast<SharedShellApp*>(app_client_.client_context);
}

void ShellApp::initAppClient()
{
    auto ptr = new SharedShellApp;
    ptr->app = shared_from_this();
    app_client_.client_context = ptr;
    app_client_.OnAppNotifyEvent = _onNotifyEvent;
    app_client_.OnNavigateToApp = _onNavigateToApp;
    app_client_.OnAppDestroyOnUI = _onAppDestroy;
    app_client_.OnFeatureManagerPrepare = _onFeatureManagerPrepare;
    app_client_.OnFeatureManagerDestroy = _onFeatureManagerDestroy;
}

void ShellApp::_onNotifyEvent(QApplicationClient* client,
    int app_event,
    void* params)
{
    auto s_shell_app_ = (SharedShellApp*)(client->client_context);
    s_shell_app_->app->onNotifyEvent(app_event, params);
}

bool ShellApp::_onNavigateToApp(QApplicationClient* client,
    const QAppRouteInfo* info)
{
    auto s_shell_app_ = (SharedShellApp*)(client->client_context);
    return s_shell_app_->app->delegate_->onNavigateToApp(info);
}

void ShellApp::_onAppDestroy(QApplicationClient* client)
{
    auto s_shell_app_ = (SharedShellApp*)(client->client_context);
    s_shell_app_->app->onAppDestroy();
}

void ShellApp::_onFeatureManagerPrepare(QApplicationClient* client)
{
    auto s_shell_app_ = (SharedShellApp*)(client->client_context);
    s_shell_app_->app->onFeatureManagerPrepare();
}

void ShellApp::_onFeatureManagerDestroy(QApplicationClient* client)
{
    auto s_shell_app_ = (SharedShellApp*)(client->client_context);
    s_shell_app_->app->onFeatureManagerDestroy();
}

void ShellApp::onNotifyEvent(int event, void* params)
{
    switch (event) {
    case QAPP_EVENT_EXIT:
        break;
    case QAPP_EVENT_REQ_EXIT:
        break;
    case QAPP_EVENT_PREPARE:
        delegate_->onSplashScreenStart(getIconPath());
        break;
    case QAPP_EVENT_DESTROY:
        js_task_queue_.reset();
        break;
    case QAPP_EVENT_FIRST_PAGE_SHOW:
        delegate_->onSplashScreenFinish();
        break;
    default:
        break;
    }

    // do observers
    for (auto& ob : observers_) {
        ob->onNotifyEvent(this, event, params);
    }
}

void ShellApp::onAppDestroy()
{
    if (gui_context_) {
        GUIWidgetContextDestroy(gui_context_);
        gui_context_ = NULL;
    }
}

void ShellApp::onFeatureManagerPrepare()
{
#ifdef CONFIG_FEATURE_RUST_MODULES
    vdk_runtime_ = init_vdk_async_runtime(app_loop_);
#endif
}

void ShellApp::onFeatureManagerDestroy()
{
#ifdef CONFIG_FEATURE_RUST_MODULES
    close_vdk_async_runtime(vdk_runtime_);
#endif
}

/**
 * @brief 如果 messageloop 完全由外部创建，这个也许没用
 */
bool ShellApp::setUILoop(uv_loop_t* loop)
{
    if (ui_loop_ == NULL) {
        ui_loop_ = loop;
        return true;
    }
    return false;
}

void ShellApp::show()
{
    if (quick_app_) {
        QApplicationShow(quick_app_);
    }
}

void ShellApp::hide()
{
    if (quick_app_) {
        QApplicationHide(quick_app_);
    }
}

void ShellApp::route(const char* url)
{
    if (quick_app_) {
        QApplicationRoute(quick_app_, url);
    }
}

void ShellApp::backpress()
{
    if (quick_app_) {
        QApplicationBackPress(quick_app_);
    }
}

}
