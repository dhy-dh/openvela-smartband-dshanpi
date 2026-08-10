#ifndef _QUICKAPP_SHELL_APP_H_
#define _QUICKAPP_SHELL_APP_H_

#include <uv.h>
// 包含相关的头文件
#include "feature_main_exports.h"
#include "gui_widget_context.h"
#include "quickapp.h"

#include <map>
#include <memory>

#include "permission/permission_manager.h"
#include "task_queue.h"

typedef struct MessageLoop MessageLoop;

namespace shell {

class ShellApp : public std::enable_shared_from_this<ShellApp> {
public:
    class Observer {
    public:
        virtual ~Observer() { }
        virtual void onNotifyEvent(ShellApp* app, int event_id, void* params) = 0;
    };
    class Delegate {
    public:
        Delegate() {};
        virtual ~Delegate() { }
        virtual bool onNavigateToApp(const QAppRouteInfo* info) { return false; }
        virtual NativeWidgetHandle transformRootWidget(NativeWidgetHandle root_ui) { return root_ui; }
        virtual void onSplashScreenStart(const std::string& icon_path) { }
        virtual void onSplashScreenFinish() {};
        virtual void initCustomFeatures(FeatureRegistryHandle hReg) {};
    };
    static std::shared_ptr<ShellApp> Create(const char* url,
        MessageLoop* ui_message_loop, std::unique_ptr<Delegate> delegate = std::make_unique<Delegate>(), bool async_mode = true)
    {
        return std::make_shared<ShellApp>(url, ui_message_loop, std::move(delegate), async_mode);
    }

    /**
     * need a delegate, to decouple splash_screen
     */
    ShellApp(const char* url, MessageLoop* ui_message_loop,
        std::unique_ptr<Delegate> delegate, bool async_mode = true);
    ~ShellApp()
    {
    }

    void create(NativeWidgetHandle root);

    bool setUILoop(uv_loop_t* loop);

    uv_loop_t* getUILoop();

    MessageLoop* getUIMessageLoop()
    {
        return ui_message_loop_;
    }

    std::shared_ptr<TaskRunner> getJSTaskRunner()
    {
        return js_task_queue_ ? js_task_queue_->GetTaskRunner()
                              : std::shared_ptr<TaskRunner>();
    }

    void release();

    void start();
    void stop();

    void show();
    void hide();
    void route(const char* url);
    void backpress();

    void runMainLoop();

    void waitUIDestroy(); // 等待UI销毁

    const std::string& getPackageName() const { return package_name_; }

    QApplicationHandle getQuickApp() const { return quick_app_; }

    void addObserver(std::unique_ptr<Observer> observer)
    {
        observers_.push_back(std::move(observer));
    }

    PermissionManager* getPermissionManager()
    {
        return permission_manager_.get();
    }

    void addFeatureManagerUserData(const std::string& key, void* data)
    {
        feature_manager_user_data_[key] = data;
    }

private:
    void onNotifyEvent(int event, void* params);
    void onAppDestroy();
    void initAppClient();
    void initApplicationAsync();
    void onFeatureManagerPrepare();
    void onFeatureManagerDestroy();

    // 初始化 gui context
    void initGUIContext(NativeWidgetHandle root, MessageLoop* ui_loop);

    // 初始化 Application
    void initApplication(MessageLoop* msg_loop);

    static void runApplication(void* arg);
    static void _onNotifyEvent(QApplicationClient* client,
        int app_event,
        void* params);
    static bool _onNavigateToApp(QApplicationClient* client,
        const QAppRouteInfo* info);
    static void _onAppDestroy(QApplicationClient* client);
    static void _onFeatureManagerPrepare(QApplicationClient* client);
    static void _onFeatureManagerDestroy(QApplicationClient* client);

    uv_loop_t* getRenderLoop();
    void initUrl(const char* url);
    std::string getIconPath();

    bool async_mode_ = false; // 是否是异步模式
    bool owned_loop_;
    QApplicationHandle quick_app_; // 快应用模块
    GUIWidgetContextHandle gui_context_; // UI接口
    uv_loop_t* ui_loop_;
    uv_loop_t* app_loop_;
    NativeWidgetHandle root_ui_;
    std::string url_;
    std::string package_name_;
    std::unique_ptr<PermissionManager> permission_manager_;
    QApplicationClient app_client_;
    std::unique_ptr<TaskQueue> js_task_queue_;
    MessageLoop* ui_message_loop_; // don't owned it
    std::map<std::string, void*> feature_manager_user_data_;
    std::vector<std::unique_ptr<Observer>> observers_;
    std::unique_ptr<Delegate> delegate_;
#ifdef CONFIG_FEATURE_RUST_MODULES
    void* vdk_runtime_;
#endif
};

}

#endif
