#pragma once

#include <stdint.h>

#define PENNYWORTH_DEFAULT_SLEEP_TIMEOUT_S 8
#define PENNYWORTH_DEFAULT_BRIGHTNESS 100

typedef struct {
  uint16_t sleep_timeout_s;
  uint8_t brightness; /* 10-100, percent */
} pennyworth_settings_t;

/* Fills *out from flash (NVS on the ESP32, a file on the simulator). If
 * nothing has been saved yet, fills defaults and writes them out, so the
 * next boot reads back the same values. */
void pennyworth_storage_load(pennyworth_settings_t *out);

void pennyworth_storage_save(const pennyworth_settings_t *settings);
