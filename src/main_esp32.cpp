/* ESP32-S3 entry point. Builds with `pio run -e esp32-s3`.
 *
 * The physical panel driver isn't wired up yet — the board hasn't arrived,
 * and CLAUDE.md says never guess pin numbers. flush_cb below is a stub so
 * this links and runs the same UI code as the simulator; Milestone 6 swaps
 * it for a real GC9A01 driver using pins verified against the Waveshare
 * wiki, in include/board_pins.h. */
#include <Arduino.h>
#include <lvgl.h>

#include "pennyworth_ui.h"

#define PENNYWORTH_SCREEN_W 240
#define PENNYWORTH_SCREEN_H 240

static uint32_t tick_get_cb(void) { return millis(); }

static void flush_cb(lv_display_t *disp, const lv_area_t *area,
                      uint8_t *px_map) {
  (void)area;
  (void)px_map;
  lv_display_flush_ready(disp);
}

void setup() {
  lv_init();
  lv_tick_set_cb(tick_get_cb);

  static lv_color_t buf1[PENNYWORTH_SCREEN_W * 40];
  lv_display_t *disp =
      lv_display_create(PENNYWORTH_SCREEN_W, PENNYWORTH_SCREEN_H);
  lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1),
                          LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_display_set_flush_cb(disp, flush_cb);

  pennyworth_ui_init();
}

void loop() {
  lv_timer_handler();
  delay(5);
}
