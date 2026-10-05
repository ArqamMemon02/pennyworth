#include "pennyworth_state.h"

#include <cstdlib>

#include "pennyworth_sprites.h"

#define WAKING_DURATION_MS 700
#define SLEEPING_DURATION_MS 500
#define AWAKE_TIMEOUT_MS 8000

#define BLINK_INTERVAL_MIN_MS 2500
#define BLINK_INTERVAL_MAX_MS 5000
#define BLINK_HOLD_MS 150

static lv_obj_t *s_sprite;
static pennyworth_state_t s_state;
static uint32_t s_state_entered_ms;
static uint32_t s_next_blink_ms;
static bool s_blinking;

static uint32_t random_blink_delay(void) {
  return BLINK_INTERVAL_MIN_MS +
         (rand() % (BLINK_INTERVAL_MAX_MS - BLINK_INTERVAL_MIN_MS));
}

static void enter_state(pennyworth_state_t state) {
  s_state = state;
  s_state_entered_ms = lv_tick_get();

  switch (state) {
    case PENNYWORTH_ASLEEP:
      lv_image_set_src(s_sprite, &pennyworth_asleep);
      break;
    case PENNYWORTH_WAKING:
      lv_image_set_src(s_sprite, &pennyworth_waking);
      break;
    case PENNYWORTH_AWAKE:
      lv_image_set_src(s_sprite, &pennyworth_awake);
      s_blinking = false;
      s_next_blink_ms = s_state_entered_ms + random_blink_delay();
      break;
    case PENNYWORTH_SLEEPING:
      /* No dedicated "lying back down" frame yet — the waking pose (sitting
       * up, eyes half open) reads fine played in reverse too. */
      lv_image_set_src(s_sprite, &pennyworth_waking);
      break;
  }
}

void pennyworth_state_init(lv_obj_t *sprite_img) {
  s_sprite = sprite_img;
  enter_state(PENNYWORTH_ASLEEP);
}

void pennyworth_state_wake_event(void) {
  if (s_state == PENNYWORTH_ASLEEP) {
    enter_state(PENNYWORTH_WAKING);
  } else if (s_state == PENNYWORTH_AWAKE) {
    s_state_entered_ms = lv_tick_get(); /* reset the inactivity timeout */
  }
  /* Taps mid-transition (WAKING/SLEEPING) are ignored. */
}

void pennyworth_state_tick(void) {
  uint32_t now = lv_tick_get();
  uint32_t elapsed = now - s_state_entered_ms; /* unsigned wraps correctly */

  switch (s_state) {
    case PENNYWORTH_WAKING:
      if (elapsed >= WAKING_DURATION_MS) enter_state(PENNYWORTH_AWAKE);
      break;

    case PENNYWORTH_AWAKE:
      if (elapsed >= AWAKE_TIMEOUT_MS) {
        enter_state(PENNYWORTH_SLEEPING);
        break;
      }
      if (!s_blinking && now >= s_next_blink_ms) {
        s_blinking = true;
        lv_image_set_src(s_sprite, &pennyworth_awake_blink);
      } else if (s_blinking && now >= s_next_blink_ms + BLINK_HOLD_MS) {
        s_blinking = false;
        lv_image_set_src(s_sprite, &pennyworth_awake);
        s_next_blink_ms = now + random_blink_delay();
      }
      break;

    case PENNYWORTH_SLEEPING:
      if (elapsed >= SLEEPING_DURATION_MS) enter_state(PENNYWORTH_ASLEEP);
      break;

    case PENNYWORTH_ASLEEP:
      break;
  }
}
