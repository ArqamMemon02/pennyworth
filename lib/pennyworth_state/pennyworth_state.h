#pragma once

#include <lvgl.h>

typedef enum {
  PENNYWORTH_ASLEEP,
  PENNYWORTH_WAKING,
  PENNYWORTH_AWAKE,
  PENNYWORTH_SLEEPING,
} pennyworth_state_t;

/* Call once, after the sprite image object exists. Starts ASLEEP. */
void pennyworth_state_init(lv_obj_t *sprite_img);

/* Call on a wake-trigger event: a double-tap in Milestone 4, a touchscreen
 * tap as the fallback CLAUDE.md calls for in the meantime. No-op unless
 * currently ASLEEP or AWAKE (mid-transition taps are ignored). */
void pennyworth_state_wake_event(void);

/* Drives transition timers and the idle blink. Call frequently (tens of
 * ms) for smooth transitions — the 1s clock tick is too coarse. */
void pennyworth_state_tick(void);

pennyworth_state_t pennyworth_state_get(void);

/* Launcher calls this when the sleep-timeout setting changes. Takes effect
 * immediately, including for the AWAKE period already in progress. */
void pennyworth_state_set_awake_timeout_ms(uint32_t ms);
