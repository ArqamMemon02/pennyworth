#include "pennyworth_ui.h"

#include <lvgl.h>
#include <time.h>
#include <stdio.h>

#include "pennyworth_state.h"
#include "pennyworth_world.h"

#define PENNYWORTH_SCREEN_DIAMETER 240
#define STATE_TICK_PERIOD_MS 50

static lv_obj_t *s_clock_label;

static void clock_timer_cb(lv_timer_t *timer) {
  (void)timer;
  time_t now = time(NULL);
  struct tm local_tm;
  localtime_r(&now, &local_tm);

  char buf[9];
  snprintf(buf, sizeof(buf), "%02d:%02d:%02d", local_tm.tm_hour,
           local_tm.tm_min, local_tm.tm_sec);
  lv_label_set_text(s_clock_label, buf);

  pennyworth_world_update(&local_tm);
}

static void state_tick_timer_cb(lv_timer_t *timer) {
  (void)timer;
  pennyworth_state_tick();
}

static void wake_on_tap_cb(lv_event_t *e) {
  (void)e;
  /* Touchscreen tap as the fallback wake trigger CLAUDE.md calls for;
   * double-tap-via-IMU is Milestone 4. */
  pennyworth_state_wake_event();
}

void pennyworth_ui_init(void) {
  lv_obj_t *scr = lv_screen_active();
  lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

  /* The panel itself is round; this mask marks the visible area so the
   * simulator (a square window) previews what the real display will show. */
  lv_obj_t *round_mask = lv_obj_create(scr);
  lv_obj_remove_style_all(round_mask);
  lv_obj_set_size(round_mask, PENNYWORTH_SCREEN_DIAMETER,
                   PENNYWORTH_SCREEN_DIAMETER);
  lv_obj_center(round_mask);
  lv_obj_set_style_radius(round_mask, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_opa(round_mask, LV_OPA_COVER, 0);
  lv_obj_set_scrollable(round_mask, false);
  lv_obj_set_clickable(round_mask, true);
  lv_obj_add_event_cb(round_mask, wake_on_tap_cb, LV_EVENT_CLICKED, NULL);

  pennyworth_world_init(round_mask);

  /* Scaled and nudged down so it (and the clock above it) both stay inside
   * the 120px-radius circle, same geometric-containment approach as the
   * world layer uses since clip_corner isn't usable here (see DEVLOG). */
  lv_obj_t *pennyworth = lv_image_create(round_mask);
  lv_image_set_scale(pennyworth, 110); /* ~43% of the source's 200x200 */
  lv_obj_align(pennyworth, LV_ALIGN_CENTER, 0, 38);
  pennyworth_state_init(pennyworth);

  s_clock_label = lv_label_create(round_mask);
  lv_obj_set_style_text_color(s_clock_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(s_clock_label, &lv_font_montserrat_28, 0);
  lv_obj_align(s_clock_label, LV_ALIGN_CENTER, 0, -48);
  lv_label_set_text(s_clock_label, "00:00:00");

  lv_timer_create(clock_timer_cb, 1000, NULL);
  clock_timer_cb(NULL);
  lv_timer_create(state_tick_timer_cb, STATE_TICK_PERIOD_MS, NULL);
}
