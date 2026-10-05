#pragma once

/* Builds the Milestone 1 test screen: a round mask matching the physical
 * 240x240 display's visible area, with the current time centered in it.
 * Hardware-agnostic — called identically from the simulator and the ESP32. */
void pennyworth_ui_init(void);
