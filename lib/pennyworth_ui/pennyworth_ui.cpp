#include "pennyworth_ui.h"

#include <lvgl.h>
#include <time.h>
#include <stdio.h>

#define PENNYWORTH_SCREEN_DIAMETER 240

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
  lv_obj_set_style_bg_color(round_mask, lv_color_hex(0x10131a), 0);
  lv_obj_set_style_bg_opa(round_mask, LV_OPA_COVER, 0);
  lv_obj_set_scrollable(round_mask, false);

  s_clock_label = lv_label_create(round_mask);
  lv_obj_set_style_text_color(s_clock_label, lv_color_white(), 0);
  lv_obj_set_style_text_font(s_clock_label, &lv_font_montserrat_28, 0);
  lv_obj_center(s_clock_label);
  lv_label_set_text(s_clock_label, "00:00:00");

  lv_timer_create(clock_timer_cb, 1000, NULL);
  clock_timer_cb(NULL);
}
