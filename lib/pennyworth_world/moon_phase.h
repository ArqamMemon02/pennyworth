#pragma once

/* Current lunar phase as a fraction of the synodic month: 0.0 = new moon,
 * 0.5 = full moon, approaching 1.0 wraps back to new moon.
 *
 * Conway's moon phase algorithm — a standard closed-form date formula
 * (no lookup tables, no calendar data). Accurate to within a day or two,
 * which is plenty for a decorative moon on a pendant. */
double moon_phase_fraction(int year, int month, int day);
