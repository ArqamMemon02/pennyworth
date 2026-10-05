#pragma once

#include <stdbool.h>
#include <stdint.h>

/* Pure double-tap detection: no hardware, no LVGL — just timestamps in,
 * confirmed-double-tap out. Easy to unit test, easy to drive from either a
 * real IMU accel-spike stream or a simulated key press. */

#define DOUBLE_TAP_WINDOW_MS 400

typedef struct {
  uint32_t last_tap_ms;
  bool has_last_tap;
} double_tap_detector_t;

void double_tap_reset(double_tap_detector_t *d);

/* Feed one raw tap impulse at time now_ms. Returns true if this tap
 * completes a double-tap (arrived within DOUBLE_TAP_WINDOW_MS of the
 * previous one) — single taps are deliberately not reported, since a
 * pendant picks up accidental bumps the device must ignore. */
bool double_tap_feed(double_tap_detector_t *d, uint32_t now_ms);
