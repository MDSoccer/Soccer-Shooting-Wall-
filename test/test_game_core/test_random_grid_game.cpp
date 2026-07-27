#include <unity.h>
#include "RandomGridGame.h"

void setUp(void) {}
void tearDown(void) {}

static uint8_t countTargets(RandomGridGame& game, uint8_t zoneCount) {
    uint8_t count = 0;
    for (uint8_t z = 0; z < zoneCount; ++z) {
        if (game.isTarget(z)) count++;
    }
    return count;
}

void test_exactly_target_count_zones_are_chosen() {
    RandomGridGame game(8, 3, 5000, /*seed*/ 42);
    game.begin(0);
    TEST_ASSERT_EQUAL_UINT8(3, countTargets(game, 8));
}

void test_different_seeds_can_pick_different_targets() {
    RandomGridGame gameA(8, 3, 5000, 1);
    RandomGridGame gameB(8, 3, 5000, 999);
    gameA.begin(0);
    gameB.begin(0);

    bool identical = true;
    for (uint8_t z = 0; z < 8; ++z) {
        if (gameA.isTarget(z) != gameB.isTarget(z)) identical = false;
    }
    TEST_ASSERT_FALSE(identical);
}

void test_hitting_a_non_target_zone_counts_as_extra() {
    RandomGridGame game(8, 1, 5000, 7);
    game.begin(0);

    uint8_t nonTarget = 255;
    for (uint8_t z = 0; z < 8; ++z) {
        if (!game.isTarget(z)) { nonTarget = z; break; }
    }
    TEST_ASSERT_TRUE(nonTarget != 255);

    game.onHit(nonTarget, 900, 10);
    TEST_ASSERT_EQUAL_UINT16(0, game.result().totalHits);
    TEST_ASSERT_EQUAL_UINT16(1, game.result().extraHits);
}

void test_clearing_all_targets_completes_round() {
    RandomGridGame game(6, 2, 5000, 3);
    game.begin(0);

    for (uint8_t z = 0; z < 6; ++z) {
        if (game.isTarget(z)) game.onHit(z, 900, 50);
    }
    game.update(50);

    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_TRUE(game.result().completed);
    TEST_ASSERT_EQUAL_UINT16(2, game.result().totalHits);
}

void test_time_expiry_ends_round_uncompleted() {
    RandomGridGame game(6, 2, 1000, 3);
    game.begin(0);
    game.update(1000);
    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_FALSE(game.result().completed);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_exactly_target_count_zones_are_chosen);
    RUN_TEST(test_different_seeds_can_pick_different_targets);
    RUN_TEST(test_hitting_a_non_target_zone_counts_as_extra);
    RUN_TEST(test_clearing_all_targets_completes_round);
    RUN_TEST(test_time_expiry_ends_round_uncompleted);
    return UNITY_END();
}
