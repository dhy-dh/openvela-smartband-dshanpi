#ifndef OPENVELA_UI_H
#define OPENVELA_UI_H

#include <stdbool.h>

void openvela_ui_create(void);

/* Reduce presentation work without stopping playback or background data. */
void openvela_ui_set_low_power(bool enabled);

#endif
