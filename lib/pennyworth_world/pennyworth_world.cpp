#include "pennyworth_world.h"

#include <cmath>
#include <cstdlib>

#include "moon_phase.h"

#define WORLD_DIAMETER 240
#define WORLD_CENTER (WORLD_DIAMETER / 2)

#define STAR_COUNT 14
#define SUN_DIAMETER 28
#define MOON_DIAMETER 46
#define CLOUD_COUNT 2
#define CLOUD_PUFFS 3

/* Background tone keyframes across a 24h day. Colors stay mid-saturation
 * (never near-white or near-black) so the white clock text stays legible
 * through every phase. */
struct tone_keyframe {
  float hour;
  lv_color_t color;
};

static const tone_keyframe TONE_KEYFRAMES[] = {
    {0.0f, {0x20, 0x10, 0x0b}},   /* deep night (b,g,r) */
    {5.0f, {0x20, 0x10, 0x0b}},   /* deep night, pre-dawn */
    {6.5f, {0x3d, 0x8a, 0xd9}},   /* warm amber morning */
    {9.0f, {0xe0, 0xa7, 0x3f}},   /* bright clean day */
    {16.0f, {0xe0, 0xa7, 0x3f}},  /* day continues */
    {18.0f, {0x3f, 0x5d, 0xcf}},  /* warm evening */
    {19.5f, {0x33, 0x1f, 0x1a}},  /* dim cool night transition */
    {22.0f, {0x20, 0x10, 0x0b}},  /* deep night */
    {24.0f, {0x20, 0x10, 0x0b}},  /* wrap */
};
#define TONE_KEYFRAME_COUNT \
  (sizeof(TONE_KEYFRAMES) / sizeof(TONE_KEYFRAMES[0]))

#define DAY_START_HOUR 6.5f
#define DAY_END_HOUR 18.0f
#define CLOUD_END_HOUR 10.0f /* clouds linger through dawn into mid-morning, then clear */

/* Each cloud is 3 overlapping puffs (left, center-top, right) — the classic
 * flat cloud-icon cluster. Positions/radii keep every puff's edge inside
 * the 120px round boundary, same containment approach as the rest of this
 * file (see DEVLOG re: clip_corner). */
struct cloud_puff {
  int16_t dx, dy, r;
};
struct cloud_def {
  int16_t cx, cy;
  cloud_puff puffs[CLOUD_PUFFS];
};
static const cloud_def CLOUD_DEFS[CLOUD_COUNT] = {
    {78, 92, {{-14, 2, 9}, {0, -4, 13}, {14, 3, 10}}},
    {168, 98, {{-13, 3, 9}, {1, -3, 12}, {14, 2, 9}}},
};

static lv_obj_t *s_bg;
static lv_obj_t *s_sun;
static lv_obj_t *s_moon_lit;
static lv_obj_t *s_moon_shadow;
static lv_obj_t *s_stars[STAR_COUNT];
static lv_obj_t *s_clouds[CLOUD_COUNT][CLOUD_PUFFS];
static bool s_is_day;
static bool s_visibility_applied = false; /* forces the first update to set visibility, regardless of what s_is_day happens to default to */
static bool s_initialized = false;

static uint8_t lerp_u8(uint8_t a, uint8_t b, float t) {
  return (uint8_t)(a + (b - a) * t);
}

static lv_color_t tone_for_hour(float hour) {
  size_t i = 0;
  while (i + 1 < TONE_KEYFRAME_COUNT && TONE_KEYFRAMES[i + 1].hour < hour) {
    i++;
  }
  const tone_keyframe &a = TONE_KEYFRAMES[i];
  const tone_keyframe &b = TONE_KEYFRAMES[i + 1];
  float span = b.hour - a.hour;
  float t = span > 0 ? (hour - a.hour) / span : 0;
  return lv_color_make(lerp_u8(a.color.red, b.color.red, t),
                        lerp_u8(a.color.green, b.color.green, t),
                        lerp_u8(a.color.blue, b.color.blue, t));
}

static void opa_anim_cb(void *obj, int32_t v) {
  lv_obj_set_style_opa((lv_obj_t *)obj, (lv_opa_t)v, 0);
}

static void twinkle(lv_obj_t *star) {
  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_var(&a, star);
  lv_anim_set_exec_cb(&a, opa_anim_cb);
  lv_anim_set_values(&a, 40 + (rand() % 60), 160 + (rand() % 95));
  lv_anim_set_time(&a, 1500 + (rand() % 2500));
  lv_anim_set_playback_time(&a, 1500 + (rand() % 2500));
  lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
  lv_anim_set_delay(&a, rand() % 2000);
  lv_anim_start(&a);
}

static lv_obj_t *make_dot(lv_obj_t *parent, int32_t diameter,
                           lv_color_t color) {
  lv_obj_t *dot = lv_obj_create(parent);
  lv_obj_remove_style_all(dot);
  lv_obj_set_size(dot, diameter, diameter);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
  lv_obj_set_style_bg_color(dot, color, 0);
  lv_obj_set_style_bg_opa(dot, LV_OPA_COVER, 0);
  lv_obj_set_scrollable(dot, false);
  return dot;
}

void pennyworth_world_init(lv_obj_t *parent) {
  /* No child clipping to the round boundary: LVGL's clip_corner style hides
   * everything (even dead-centered children) on this renderer rather than
   * just cropping the four corners, so every element below is positioned
   * with enough margin to stay geometrically inside the circle instead. */
  s_bg = parent;

  /* Stars: fixed pseudo-random positions inside the circle, each twinkling
   * on its own randomized cycle. Seeded so the layout is stable session to
   * session rather than reshuffling on every boot. */
  srand(1);
  for (int i = 0; i < STAR_COUNT; i++) {
    double angle = (rand() % 360) * (M_PI / 180.0);
    double radius = (rand() % (WORLD_CENTER - 16)) + 4;
    int x = WORLD_CENTER + (int)(radius * cos(angle));
    int y = WORLD_CENTER + (int)(radius * sin(angle));

    lv_obj_t *star = make_dot(parent, 2 + (rand() % 2), lv_color_white());
    lv_obj_set_pos(star, x, y);
    s_stars[i] = star;
    twinkle(star);
  }

  /* Sun: fixed decorative corner position (not a real sun position), placed
   * at radius 90 from center so its full 28px disc clears the 120px edge. */
  s_sun = make_dot(parent, SUN_DIAMETER, lv_color_make(0xff, 0xd3, 0x66));
  lv_obj_set_pos(s_sun, 42, 42);

  /* Moon: two overlapping discs. The shadow disc slides across the lit disc
   * by up to half a diameter; where they overlap reads as the dark side,
   * the exposed sliver of the lit disc as the phase. Positioned at radius
   * 70 from center so even the shadow's maximum excursion stays inside the
   * 120px edge (worst case ~111px from center, see DEVLOG). */
  s_moon_lit = make_dot(parent, MOON_DIAMETER, lv_color_make(0xe8, 0xe8, 0xec));
  lv_obj_set_pos(s_moon_lit, 147, 48);

  /* Shadow's color is set every update() call to match the live sky tone
   * (see below) so the dark side reads as "unlit," not as a black patch
   * painted over the sky — lv_color_white() here is just a placeholder
   * until the first update() call overwrites it. */
  s_moon_shadow = make_dot(parent, MOON_DIAMETER, lv_color_white());

  /* Clouds: dawn/morning only (see CLOUD_END_HOUR). Soft warm-white, not
   * fully opaque, so the amber dawn tone shows through. */
  for (int c = 0; c < CLOUD_COUNT; c++) {
    const cloud_def &def = CLOUD_DEFS[c];
    for (int p = 0; p < CLOUD_PUFFS; p++) {
      const cloud_puff &puff = def.puffs[p];
      lv_obj_t *dot =
          make_dot(parent, puff.r * 2, lv_color_make(0xff, 0xf6, 0xe6));
      lv_obj_set_style_bg_opa(dot, 210, 0);
      lv_obj_set_pos(dot, def.cx + puff.dx - puff.r, def.cy + puff.dy - puff.r);
      s_clouds[c][p] = dot;
    }
  }

  s_initialized = true;
}

void pennyworth_world_update(const struct tm *now) {
  if (!s_initialized) return;

  float hour = now->tm_hour + now->tm_min / 60.0f;
  lv_color_t tone = tone_for_hour(hour);
  lv_obj_set_style_bg_color(s_bg, tone, 0);

  /* The moon's dark side is "unlit," not painted black — matching the live
   * sky tone makes it blend away instead of looking like a hole cut in the
   * background. */
  lv_obj_set_style_bg_color(s_moon_shadow, tone, 0);

  bool is_day = hour >= DAY_START_HOUR && hour < DAY_END_HOUR;
  if (!s_visibility_applied || is_day != s_is_day) {
    s_visibility_applied = true;
    s_is_day = is_day;
    lv_obj_set_hidden(s_sun, !is_day);
    lv_obj_set_hidden(s_moon_lit, is_day);
    lv_obj_set_hidden(s_moon_shadow, is_day);
    for (int i = 0; i < STAR_COUNT; i++) lv_obj_set_hidden(s_stars[i], is_day);
  }

  bool is_cloudy = is_day && hour < CLOUD_END_HOUR;
  for (int c = 0; c < CLOUD_COUNT; c++)
    for (int p = 0; p < CLOUD_PUFFS; p++)
      lv_obj_set_hidden(s_clouds[c][p], !is_cloudy);

  double phase = moon_phase_fraction(now->tm_year + 1900, now->tm_mon + 1,
                                      now->tm_mday);
  double offset_frac = (1.0 - cos(2.0 * M_PI * phase)) / 2.0; /* 0..1 */
  double direction = (phase < 0.5) ? 1.0 : -1.0;
  int offset_px = (int)lround(direction * offset_frac * MOON_DIAMETER);

  lv_obj_t *lit_parent_coords = s_moon_lit;
  lv_coord_t lit_x = lv_obj_get_x(lit_parent_coords);
  lv_coord_t lit_y = lv_obj_get_y(lit_parent_coords);
  lv_obj_set_pos(s_moon_shadow, lit_x + offset_px, lit_y);
}
