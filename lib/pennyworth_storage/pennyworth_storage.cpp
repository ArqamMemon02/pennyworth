#include "pennyworth_storage.h"

#ifdef PENNYWORTH_SIMULATOR

#include <cstdio>

#define STORAGE_FILE "pennyworth_state.dat"
#define STORAGE_MAGIC \
  0x504e5732u /* "PNW2" — bumped when brightness was added to the struct */

struct storage_blob {
  uint32_t magic;
  pennyworth_settings_t settings;
};

void pennyworth_storage_save(const pennyworth_settings_t *settings) {
  storage_blob blob{STORAGE_MAGIC, *settings};
  FILE *f = fopen(STORAGE_FILE, "wb");
  if (!f) return;
  fwrite(&blob, sizeof(blob), 1, f);
  fclose(f);
}

void pennyworth_storage_load(pennyworth_settings_t *out) {
  storage_blob blob;
  FILE *f = fopen(STORAGE_FILE, "rb");
  bool ok =
      f && fread(&blob, sizeof(blob), 1, f) == 1 && blob.magic == STORAGE_MAGIC;
  if (f) fclose(f);

  if (ok) {
    *out = blob.settings;
    return;
  }

  out->sleep_timeout_s = PENNYWORTH_DEFAULT_SLEEP_TIMEOUT_S;
  out->brightness = PENNYWORTH_DEFAULT_BRIGHTNESS;
  pennyworth_storage_save(out);
}

#else

#include <Preferences.h>

static Preferences s_prefs;

void pennyworth_storage_save(const pennyworth_settings_t *settings) {
  s_prefs.begin("pennyworth", false);
  s_prefs.putUShort("sleep_s", settings->sleep_timeout_s);
  s_prefs.putUChar("bright", settings->brightness);
  s_prefs.end();
}

void pennyworth_storage_load(pennyworth_settings_t *out) {
  s_prefs.begin("pennyworth", false);
  out->sleep_timeout_s =
      s_prefs.getUShort("sleep_s", PENNYWORTH_DEFAULT_SLEEP_TIMEOUT_S);
  out->brightness = s_prefs.getUChar("bright", PENNYWORTH_DEFAULT_BRIGHTNESS);
  s_prefs.end();
}

#endif
