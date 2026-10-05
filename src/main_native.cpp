/* Desktop simulator entry point. Builds with `pio run -e native -t exec`.
 * Renders the same UI code the ESP32 will run, in an SDL window. */
#include <SDL.h>
#include <lvgl.h>

#include "pennyworth_ui.h"

int main(void) {
  lv_init();

  lv_display_t *disp = lv_sdl_window_create(240, 240);
  lv_sdl_window_set_resizeable(disp, false);
  lv_sdl_mouse_create();

  pennyworth_ui_init();

  while (1) {
    uint32_t sleep_ms = lv_timer_handler();
    SDL_Delay(sleep_ms ? (sleep_ms < 10 ? sleep_ms : 10) : 1);
  }
}
