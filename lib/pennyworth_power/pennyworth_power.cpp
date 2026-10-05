#include "pennyworth_power.h"

#ifdef PENNYWORTH_SIMULATOR

/* No real sleep state to enter on a desktop window; screen dimming becomes
 * visible once Milestone 5 adds a brightness concept. */
void pennyworth_power_enter_sleep(void) {}
void pennyworth_power_enter_wake(void) {}

#else

/* TODO(Milestone 6): once include/board_pins.h exists with a verified IMU
 * interrupt pin, configure esp_sleep_enable_ext0_wakeup() on that GPIO and
 * call esp_light_sleep_start() / esp_deep_sleep_start() from
 * pennyworth_power_enter_sleep(). Left as a no-op until the pin is known —
 * CLAUDE.md says never guess pin numbers. */
void pennyworth_power_enter_sleep(void) {}
void pennyworth_power_enter_wake(void) {}

#endif
