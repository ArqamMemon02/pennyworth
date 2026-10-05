#include "moon_phase.h"

#include <cmath>

double moon_phase_fraction(int year, int month, int day) {
  int r = year % 100;
  r %= 19;
  if (r > 9) r -= 19;

  double v = (double)((r * 11) % 30) + month + day;
  if (month < 3) v += 2;
  v -= (year < 2000) ? 4.0 : 8.3;

  long whole = std::lround(v);
  int age_days = (int)(((whole % 30) + 30) % 30); /* 0..29, 0 = new moon */

  return age_days / 30.0;
}
