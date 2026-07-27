#include <unity.h>
#include "TwoPlayerSplitGame.h"

void setUp(void) {}
void tearDown(void) {}

// zoneCount=4, leftCount=2 -> zones 0,1 = left player, zones 2,3 = right player.

void test_left_player_wins_by_clearing_first() {
    TwoPlayerSplitGame game(4, 2, 5000);
    game.begin(0);

    game.onHit(0, 900, 100);
    game.onHit(1, 900, 150);
    game.update(150);

    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_TRUE(game.result().completed);
    TEST_ASSERT_EQUAL_INT(0, game.result().winner);
}

void test_right_player_wins_by_clearing_first() {
    TwoPlayerSplitGame game(4, 2, 5000);
    game.begin(0);

    game.onHit(2, 900, 100);
    game.onHit(3, 900, 150);
    game.update(150);

    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_EQUAL_INT(1, game.result().winner);
}

void test_time_expiry_awards_more_zones_cleared() {
    TwoPlayerSplitGame game(4, 2, 1000);
    game.begin(0);

    game.onHit(0, 900, 100); // left clears 1 of 2
    // right clears 0 of 2
    game.update(1000);

    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_FALSE(game.result().completed);
    TEST_ASSERT_EQUAL_INT(0, game.result().winner);
}

void test_time_expiry_with_equal_clears_is_a_draw() {
    TwoPlayerSplitGame game(4, 2, 1000);
    game.begin(0);

    game.onHit(0, 900, 100); // left clears 1 of 2
    game.onHit(2, 900, 100); // right clears 1 of 2
    game.update(1000);

    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_EQUAL_INT(2, game.result().winner);
}

void test_rehitting_cleared_zone_counts_as_extra() {
    TwoPlayerSplitGame game(4, 2, 5000);
    game.begin(0);

    game.onHit(0, 900, 100);
    game.onHit(0, 900, 150);

    TEST_ASSERT_EQUAL_UINT16(1, game.result().totalHits);
    TEST_ASSERT_EQUAL_UINT16(1, game.result().extraHits);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_left_player_wins_by_clearing_first);
    RUN_TEST(test_right_player_wins_by_clearing_first);
    RUN_TEST(test_time_expiry_awards_more_zones_cleared);
    RUN_TEST(test_time_expiry_with_equal_clears_is_a_draw);
    RUN_TEST(test_rehitting_cleared_zone_counts_as_extra);
    return UNITY_END();
}
