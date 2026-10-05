#include <unity.h>

#include <cmath>

#include "moon_phase.h"

void setUp(void) {}
void tearDown(void) {}

/* Shortest distance between two phase fractions on the 0..1 wheel. */
static double circular_distance(double a, double b) {
  double d = fabs(a - b);
  return d > 0.5 ? 1.0 - d : d;
}

static void test_known_new_moons(void) {
  /* Conway's algorithm is accurate to within ~1-2 days (~0.03-0.07 of a
   * synodic month); 0.1 leaves headroom. */
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2024, 1, 11), 0.0));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2024, 2, 9), 0.0));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2024, 3, 10), 0.0));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2000, 1, 6), 0.0));
}

static void test_known_full_moons(void) {
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2024, 1, 25), 0.5));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2024, 2, 24), 0.5));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2024, 3, 25), 0.5));
  TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, circular_distance(moon_phase_fraction(2000, 1, 21), 0.5));
}

static void test_fraction_is_in_range(void) {
  for (int m = 1; m <= 12; m++) {
    double f = moon_phase_fraction(2026, m, 15);
    TEST_ASSERT_TRUE(f >= 0.0 && f < 1.0);
  }
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_known_new_moons);
  RUN_TEST(test_known_full_moons);
  RUN_TEST(test_fraction_is_in_range);
  return UNITY_END();
}
