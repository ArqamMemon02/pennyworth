#include "double_tap.h"

void double_tap_reset(double_tap_detector_t *d) {
  d->has_last_tap = false;
  d->last_tap_ms = 0;
}

bool double_tap_feed(double_tap_detector_t *d, uint32_t now_ms) {
  if (d->has_last_tap && (now_ms - d->last_tap_ms) <= DOUBLE_TAP_WINDOW_MS) {
    /* Confirmed double-tap. Reset rather than leaving this tap armed as a
     * new "first tap" — otherwise three quick taps would register as two
     * double-taps instead of one. */
    double_tap_reset(d);
    return true;
  }

  d->last_tap_ms = now_ms;
  d->has_last_tap = true;
  return false;
}
