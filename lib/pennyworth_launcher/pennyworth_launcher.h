#pragma once

#include <lvgl.h>

/* Minimal settings launcher: a gear icon, reachable only while AWAKE, opens
 * a small panel with brightness and sleep-timeout sliders. Both apply live
 * and persist via pennyworth_storage. No other screens — CLAUDE.md asks
 * for "only Pennyworth's screens + settings," nothing else. */
void pennyworth_launcher_init(lv_obj_t *round_mask);

/* Shows/hides the gear icon based on the current Pennyworth state. Call
 * alongside pennyworth_state_tick(). */
void pennyworth_launcher_tick(void);
