#pragma once

#include <lvgl.h>
#include <time.h>

/* One consistent day/night world, drawn as children of `parent` (expected
 * to be the round mask, so stars/sun/moon near the edge get clipped to the
 * circle for free). Call once. */
void pennyworth_world_init(lv_obj_t *parent);

/* Call periodically (once a second is plenty) with the current local time.
 * Re-tones the background and repositions the moon; sun/star visibility
 * flips at the day/night boundary. */
void pennyworth_world_update(const struct tm *now);
