/* Desktop simulator entry point. Builds with `pio run -e native -t exec`.
 * Renders the same UI code the ESP32 will run, in an SDL window. */
#include <SDL.h>
#include <lvgl.h>

#include "pennyworth_input.h"
#include "pennyworth_ui.h"

/* Simulated IMU double-tap: space bar stands in for an accel-spike impulse.
 * SDL_GetKeyboardState reads live key state (a side effect of the SDL
 * event pump LVGL's own SDL driver already runs each frame) rather than
 * polling SDL_PollEvent ourselves, which would race LVGL's driver for the
 * same event queue. */
static void poll_simulated_tap_key(void) {
  static bool was_down = false;
  const Uint8 *keys = SDL_GetKeyboardState(NULL);
  bool is_down = keys[SDL_SCANCODE_SPACE];
  if (is_down && !was_down) pennyworth_input_raw_tap();
  was_down = is_down;
}

int main(void) {
  lv_init();

  lv_display_t *disp = lv_sdl_window_create(240, 240);
  lv_sdl_window_set_resizeable(disp, false);
  lv_sdl_mouse_create();

  pennyworth_ui_init();

  while (1) {
    uint32_t sleep_ms = lv_timer_handler();
    poll_simulated_tap_key();
    SDL_Delay(sleep_ms ? (sleep_ms < 10 ? sleep_ms : 10) : 1);
  }
}
