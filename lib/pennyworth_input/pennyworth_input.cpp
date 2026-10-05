#include "pennyworth_input.h"

#include <lvgl.h>

#include "double_tap.h"
#include "pennyworth_state.h"

static double_tap_detector_t s_detector;

void pennyworth_input_init(void) { double_tap_reset(&s_detector); }

void pennyworth_input_raw_tap(void) {
  if (double_tap_feed(&s_detector, lv_tick_get())) {
    pennyworth_state_wake_event();
  }
}
