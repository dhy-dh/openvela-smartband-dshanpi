#include "quickapp.h"

#include "feature_exports.h"

#include <ash/message_loop/cmessage_loop.h>

#include <lvgl.h>
#include <uikit/uikit.h>

void onNotifyEvent(QApplicationClient* client,
    int app_event,
    void* params)
{
}
void onAppDestroy(QApplicationClient* client)
{
}

extern FeatureRegistryTableHandle g_ajs_features_registry;
int regist_quickapp_features(QApplicationHandle handle,
    FeatureRegistryHandle hRegistry,
    void* user_data)
{
    FeatureRegisterFeatures(hRegistry, g_ajs_features_registry);
    return 0;
}

int feature_manager_set(QApplicationHandle handle, FeatureManagerHandle feature_manager, void* user_data)
{
    return 0;
}

int main(int argc, const char** argv)
{
    if (argc < 2) {
        printf("usage: %s <package-uri>\n", argv[0]);
        return 0;
    }

    // initialize the lvgl
    GUIWidgetInit();
    lv_init();

    lv_nuttx_dsc_t info;
    lv_nuttx_result_t result;

    lv_nuttx_dsc_init(&info);
    lv_nuttx_init(&info, &result);
    vg_init();

    uv_loop_t loop;
    uv_loop_init(&loop);

    GuiDataHandle gui_loop_data = GUILoopStart(&loop);

    MessageLoop* ui_message_loop = MessageLoop_CreateForUV(&loop);

    GUIWidgetContextCreateInfo widget_info = { 0 };
    widget_info.gui_root = (NativeWidgetHandle)lv_scr_act();

    widget_info.package_name = argv[1];
    widget_info.design_width = 480;
    widget_info.design_height = 480;
    widget_info.async_mode = false; // async mode

    GUIWidgetContextHandle gui_context = GUIWidgetContextCreate(&widget_info);

    QApplicationCreateInfo app_info = { 0 };

    QApplicationClient app_client;
    app_client.client_context = NULL;
    app_client.OnAppNotifyEvent = onNotifyEvent;
    app_client.OnNavigateToApp = NULL;
    app_client.OnAppDestroyOnUI = onAppDestroy;

    app_info.uri = argv[1];
    app_info.client = &app_client;
    app_info.gui_context = gui_context;
    app_info.message_loop = ui_message_loop; // message_loop for js runtime
    app_info.stack_size = CONFIG_QUICKAPP_THREADSTACKSIZE;
    app_info.heap_size = CONFIG_QUICKAPP_JSHEAPSIZE;
    app_info.entry = NULL;
    app_info.ui_message_loop = ui_message_loop;
    app_info.feature_register = regist_quickapp_features;
    app_info.feature_manager_set = feature_manager_set;

    QApplicationHandle quick_app = QApplicationCreate(&app_info);

    QApplicationStart(quick_app);
    uv_run(&loop, UV_RUN_DEFAULT);

    MessageLoop_Destroy(ui_message_loop);

    GUILoopStop(gui_loop_data);

    vg_deinit();
    lv_deinit();
    GUIWidgetUninit();

    return 0;
}
