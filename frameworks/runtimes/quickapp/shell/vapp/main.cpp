#include "quickapp.h"

#include "feature_exports.h"

#include <ash/message_loop/cmessage_loop.h>

#include "common/shell_app.h"

#include <lvgl.h>
#include <uikit/uikit.h>

extern "C" int main(int argc, const char** argv)
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

    auto shell_app = shell::ShellApp::Create(argv[1], ui_message_loop, std::make_unique<shell::ShellApp::Delegate>(), false);
    shell_app->setUILoop(&loop);
    shell_app->create((NativeWidgetHandle)lv_scr_act());
    shell_app->runMainLoop();
    MessageLoop_Destroy(ui_message_loop);

    GUILoopStop(gui_loop_data);

    GUIWidgetUninit();

    return 0;
}
