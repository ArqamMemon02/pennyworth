#pragma once

/* Thin interface over the platform's sleep/wake power state. The state
 * machine calls these on entering ASLEEP / leaving it; what actually
 * happens behind them is platform-specific (see pennyworth_power.cpp). */
void pennyworth_power_enter_sleep(void);
void pennyworth_power_enter_wake(void);
