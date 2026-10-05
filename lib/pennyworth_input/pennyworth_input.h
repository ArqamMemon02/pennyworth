#pragma once

/* Thin glue between a raw tap source (real IMU accel-spike detection on
 * hardware, a simulated key press on desktop) and the double-tap detector
 * + state machine. Call pennyworth_input_init() once, then
 * pennyworth_input_raw_tap() on every raw tap impulse. */
void pennyworth_input_init(void);
void pennyworth_input_raw_tap(void);
