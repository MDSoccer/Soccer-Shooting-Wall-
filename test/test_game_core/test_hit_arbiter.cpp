#include <unity.h>
#include "HitArbiter.h"

void setUp(void) {}
void tearDown(void) {}

void test_single_hit_over_threshold() {
    HitArbiter arb(4, 500, 15, 150);
    uint16_t readings[4] = {0, 0, 0, 0};

    HitEvent e = arb.feed(readings, 0);
    TEST_ASSERT_FALSE(e.valid);

    readings[2] = 800;
    e = arb.feed(readings, 5); // window just opened, not finalized yet
    TEST_ASSERT_FALSE(e.valid);

    e = arb.feed(readings, 20); // 15ms since window opened -> finalizes
    TEST_ASSERT_TRUE(e.valid);
    TEST_ASSERT_EQUAL_UINT8(2, e.zone);
    TEST_ASSERT_EQUAL_UINT16(800, e.amplitude);
}

void test_arbitration_picks_strongest_within_window() {
    HitArbiter arb(4, 500, 15, 150);
    uint16_t readings[4] = {0, 0, 0, 0};

    readings[0] = 600; // zone 0 crosses threshold first, opens the window
    arb.feed(readings, 0);

    readings[0] = 0;
    readings[1] = 900; // stronger neighbor crosstalk arrives within the window
    arb.feed(readings, 5);

    HitEvent e = arb.feed(readings, 16);
    TEST_ASSERT_TRUE(e.valid);
    TEST_ASSERT_EQUAL_UINT8(1, e.zone);
    TEST_ASSERT_EQUAL_UINT16(900, e.amplitude);
}

void test_lockout_suppresses_ringing_then_expires() {
    HitArbiter arb(2, 500, 10, 150);
    uint16_t readings[2] = {800, 0};

    arb.feed(readings, 0);
    HitEvent first = arb.feed(readings, 10);
    TEST_ASSERT_TRUE(first.valid);
    TEST_ASSERT_EQUAL_UINT8(0, first.zone);

    // Sensor still ringing right after the hit - must be ignored during lockout.
    HitEvent duringLockout = arb.feed(readings, 20);
    TEST_ASSERT_FALSE(duringLockout.valid);

    // Once the lockout window passes, a new impact registers again.
    HitEvent afterLockout;
    for (uint32_t t = 165; t <= 180; ++t) {
        afterLockout = arb.feed(readings, t);
        if (afterLockout.valid) break;
    }
    TEST_ASSERT_TRUE(afterLockout.valid);
    TEST_ASSERT_EQUAL_UINT8(0, afterLockout.zone);
}

void test_readings_below_threshold_never_trigger() {
    HitArbiter arb(2, 500, 10, 150);
    uint16_t readings[2] = {200, 499};

    for (uint32_t t = 0; t <= 50; t += 5) {
        HitEvent e = arb.feed(readings, t);
        TEST_ASSERT_FALSE(e.valid);
    }
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_single_hit_over_threshold);
    RUN_TEST(test_arbitration_picks_strongest_within_window);
    RUN_TEST(test_lockout_suppresses_ringing_then_expires);
    RUN_TEST(test_readings_below_threshold_never_trigger);
    return UNITY_END();
}
