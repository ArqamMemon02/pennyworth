#pragma once

#include <stdint.h>

#define PENNYWORTH_DEFAULT_SLEEP_TIMEOUT_S 8

typedef struct {
  uint16_t sleep_timeout_s;
} pennyworth_settings_t;

/* Fills *out from flash (NVS on the ESP32, a file on the simulator). If
 * nothing has been saved yet, fills defaults and writes them out, so the
 * next boot reads back the same values. */
void pennyworth_storage_load(pennyworth_settings_t *out);

void pennyworth_storage_save(const pennyworth_settings_t *settings);
