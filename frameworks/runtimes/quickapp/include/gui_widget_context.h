#ifndef _QUICKAPP_GUI_WIDGET_CONTEXT_H_
#define _QUICKAPP_GUI_WIDGET_CONTEXT_H_

#include <uv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MessageLoop MessageLoop;

typedef struct GUIWidgetContext* GUIWidgetContextHandle;

typedef struct _NativeWidget* NativeWidgetHandle;

typedef struct GuiData* GuiDataHandle;

typedef struct _GUIWidgetContextCreateInfo {
    NativeWidgetHandle gui_root; // native widget的root入口
    MessageLoop* ui_message_loop; // message loop 入口
    const char* package_name;
    int design_width;
    int design_height;
    bool async_mode;
} GUIWidgetContextCreateInfo;

void GUIWidgetInit(void);

void GUIWidgetUninit(void);

GuiDataHandle GUILoopStart(uv_loop_t* loop);

void GUILoopStop(GuiDataHandle gui_data);

// 创建Context部分
GUIWidgetContextHandle GUIWidgetContextCreate(const GUIWidgetContextCreateInfo* info);

void* GUIWidgetContextGetNativeHandle(GUIWidgetContextHandle handle);

void* GUIWidgetContextGetScreen(GUIWidgetContextHandle handle);

void GUIWidgetContextDestroy(GUIWidgetContextHandle handle);

bool GUIWidgetContextCanIUse(const char* prop);

#ifdef __cplusplus
}
#endif

#endif
