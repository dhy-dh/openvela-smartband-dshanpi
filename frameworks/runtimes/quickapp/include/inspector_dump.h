#ifndef _QUICKAPP_INSPECTOR_HOST_DUMP_H_
#define _QUICKAPP_INSPECTOR_HOST_DUMP_H_

#include "quickapp.h"

#ifdef __cplusplus
extern "C" {
#endif

// jse_run_heap_dump
void InspectorDumpAppHeap(QApplicationHandle app, const char* name);
void InspectorDumpJSMem(QApplicationHandle app);
void InspectorStartAppHeapTimeline(QApplicationHandle app, const char* name);
void InspectorStopAppHeapTimeline(QApplicationHandle app);
void InspectorStartAppCPUProfiling(QApplicationHandle app, const char* name);
void InspectorStopAppCPUProfiling(QApplicationHandle app);

#ifdef __cplusplus
}
#endif

#endif
