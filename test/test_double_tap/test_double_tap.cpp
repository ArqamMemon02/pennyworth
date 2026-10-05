#include <unity.h>

#include "double_tap.h"

void setUp(void) {}
void tearDown(void) {}

static void test_two_taps_within_window_confirm(void) {
  double_tap_detector_t d;
  double_tap_reset(&d);

  TEST_ASSERT_FALSE(double_tap_feed(&d, 1000));
  TEST_ASSERT_TRUE(double_tap_feed(&d, 1000 + DOUBLE_TAP_WINDOW_MS));
}

static void test_two_taps_outside_window_do_not_confirm(void) {
  double_tap_detector_t d;
  double_tap_reset(&d);

  TEST_ASSERT_FALSE(double_tap_feed(&d, 1000));
  TEST_ASSERT_FALSE(double_tap_feed(&d, 1000 + DOUBLE_TAP_WINDOW_MS + 1));
}

static void test_lone_tap_never_confirms(void) {
  double_tap_detector_t d;
  double_tap_reset(&d);

  TEST_ASSERT_FALSE(double_tap_feed(&d, 5000));
}

static void test_triple_tap_confirms_once_not_twice(void) {
  double_tap_detector_t d;
  double_tap_reset(&d);

  TEST_ASSERT_FALSE(double_tap_feed(&d, 0));
  TEST_ASSERT_TRUE(double_tap_feed(&d, 100));   /* confirms, resets */
  TEST_ASSERT_FALSE(double_tap_feed(&d, 150));  /* re-arms as a fresh tap */
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_two_taps_within_window_confirm);
  RUN_TEST(test_two_taps_outside_window_do_not_confirm);
  RUN_TEST(test_lone_tap_never_confirms);
  RUN_TEST(test_triple_tap_confirms_once_not_twice);
  return UNITY_END();
}
