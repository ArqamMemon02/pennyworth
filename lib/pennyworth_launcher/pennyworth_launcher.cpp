#include "pennyworth_launcher.h"

#include <cstdio>

#include "pennyworth_state.h"
#include "pennyworth_storage.h"

#define BRIGHTNESS_MIN 10
#define BRIGHTNESS_MAX 100
#define SLEEP_TIMEOUT_MIN_S 3
#define SLEEP_TIMEOUT_MAX_S 30

static lv_obj_t *s_icon;
static lv_obj_t *s_dim_overlay;
static lv_obj_t *s_panel;
static lv_obj_t *s_brightness_label;
static lv_obj_t *s_timeout_label;
static pennyworth_settings_t s_settings;

static void apply_brightness(uint8_t pct) {
  /* Backlight PWM is Milestone 6 (needs a verified pin); on the simulator
   * a black overlay on top of everything fakes dimming visually. */
  lv_opa_t opa = (lv_opa_t)((100 - pct) * 255 / 100);
  lv_obj_set_style_bg_opa(s_dim_overlay, opa, 0);
}

static void save_settings(void) { pennyworth_storage_save(&s_settings); }

static void brightness_slider_cb(lv_event_t *e) {
  lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
  s_settings.brightness = (uint8_t)lv_slider_get_value(slider);
  apply_brightness(s_settings.brightness);

  char buf[24];
  snprintf(buf, sizeof(buf), "Brightness  %d%%", s_settings.brightness);
  lv_label_set_text(s_brightness_label, buf);

  if (lv_event_get_code(e) == LV_EVENT_RELEASED) save_settings();
}

static void timeout_slider_cb(lv_event_t *e) {
  lv_obj_t *slider = (lv_obj_t *)lv_event_get_target(e);
  s_settings.sleep_timeout_s = (uint16_t)lv_slider_get_value(slider);
  pennyworth_state_set_awake_timeout_ms(s_settings.sleep_timeout_s * 1000u);

  char buf[24];
  snprintf(buf, sizeof(buf), "Sleep after  %ds", s_settings.sleep_timeout_s);
  lv_label_set_text(s_timeout_label, buf);

  if (lv_event_get_code(e) == LV_EVENT_RELEASED) save_settings();
}

static void open_panel_cb(lv_event_t *e) {
  (void)e;
  lv_obj_set_hidden(s_panel, false);
  lv_obj_set_hidden(s_icon, true);
}

static void close_panel_cb(lv_event_t *e) {
  (void)e;
  lv_obj_set_hidden(s_panel, true);
  /* pennyworth_launcher_tick() re-shows the icon next tick if still AWAKE */
}

static lv_obj_t *make_slider_row(lv_obj_t *parent, lv_obj_t **label_out,
                                  int32_t y, int32_t min, int32_t max,
                                  int32_t value, lv_event_cb_t cb) {
  lv_obj_t *label = lv_label_create(parent);
  lv_obj_set_style_text_color(label, lv_color_white(), 0);
  lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, y);
  *label_out = label;

  lv_obj_t *slider = lv_slider_create(parent);
  lv_obj_set_size(slider, 130, 10);
  lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, y + 20);
  lv_slider_set_range(slider, min, max);
  lv_slider_set_value(slider, value, LV_ANIM_OFF);
  lv_obj_add_event_cb(slider, cb, LV_EVENT_VALUE_CHANGED, NULL);
  lv_obj_add_event_cb(slider, cb, LV_EVENT_RELEASED, NULL);
  return slider;
}

void pennyworth_launcher_init(lv_obj_t *round_mask) {
  pennyworth_storage_load(&s_settings);

  /* Dimming overlay sits above the world + sprite + clock, below the panel
   * — created here so it's last (and therefore topmost) among those. */
  s_dim_overlay = lv_obj_create(round_mask);
  lv_obj_remove_style_all(s_dim_overlay);
  lv_obj_set_size(s_dim_overlay, lv_pct(100), lv_pct(100));
  lv_obj_set_style_bg_color(s_dim_overlay, lv_color_black(), 0);
  lv_obj_set_scrollable(s_dim_overlay, false);
  lv_obj_set_clickable(s_dim_overlay, false); /* don't block taps under it */
  apply_brightness(s_settings.brightness);

  /* Gear icon, reachable only while AWAKE (see pennyworth_launcher_tick). */
  s_icon = lv_label_create(round_mask);
  lv_obj_set_style_text_color(s_icon, lv_color_white(), 0);
  lv_obj_set_style_text_font(s_icon, &lv_font_montserrat_14, 0);
  lv_label_set_text(s_icon, LV_SYMBOL_SETTINGS);
  lv_obj_align(s_icon, LV_ALIGN_CENTER, 0, 92);
  lv_obj_set_clickable(s_icon, true);
  lv_obj_add_event_cb(s_icon, open_panel_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_set_hidden(s_icon, true);

  /* Settings panel — undersized relative to the full circle (170x150 at
   * center => farthest corner ~113px from center) so it stays inside the
   * 120px-radius boundary with square corners, no clipping needed. */
  s_panel = lv_obj_create(round_mask);
  lv_obj_remove_style_all(s_panel);
  lv_obj_set_size(s_panel, 170, 150);
  lv_obj_center(s_panel);
  lv_obj_set_style_radius(s_panel, 16, 0);
  lv_obj_set_style_bg_color(s_panel, lv_color_hex(0x15192a), 0);
  lv_obj_set_style_bg_opa(s_panel, LV_OPA_COVER, 0);
  lv_obj_set_scrollable(s_panel, false);
  lv_obj_set_hidden(s_panel, true);

  lv_obj_t *title = lv_label_create(s_panel);
  lv_obj_set_style_text_color(title, lv_color_white(), 0);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
  lv_label_set_text(title, "Settings");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 6);

  make_slider_row(s_panel, &s_brightness_label, 26, BRIGHTNESS_MIN,
                   BRIGHTNESS_MAX, s_settings.brightness, brightness_slider_cb);
  make_slider_row(s_panel, &s_timeout_label, 80, SLEEP_TIMEOUT_MIN_S,
                   SLEEP_TIMEOUT_MAX_S, s_settings.sleep_timeout_s,
                   timeout_slider_cb);

  lv_obj_t *close_btn = lv_label_create(s_panel);
  lv_obj_set_style_text_color(close_btn, lv_color_white(), 0);
  lv_obj_set_style_text_font(close_btn, &lv_font_montserrat_14, 0);
  lv_label_set_text(close_btn, LV_SYMBOL_CLOSE);
  lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -6, 4);
  lv_obj_set_clickable(close_btn, true);
  lv_obj_add_event_cb(close_btn, close_panel_cb, LV_EVENT_CLICKED, NULL);

  /* Label text starts generic from make_slider_row; set the real values. */
  char buf[24];
  snprintf(buf, sizeof(buf), "Brightness  %d%%", s_settings.brightness);
  lv_label_set_text(s_brightness_label, buf);
  snprintf(buf, sizeof(buf), "Sleep after  %ds", s_settings.sleep_timeout_s);
  lv_label_set_text(s_timeout_label, buf);
}

void pennyworth_launcher_tick(void) {
  bool awake = pennyworth_state_get() == PENNYWORTH_AWAKE;
  bool panel_open = !lv_obj_is_hidden(s_panel);
  lv_obj_set_hidden(s_icon, panel_open || !awake);
  if (!awake && panel_open) lv_obj_set_hidden(s_panel, true);
}
