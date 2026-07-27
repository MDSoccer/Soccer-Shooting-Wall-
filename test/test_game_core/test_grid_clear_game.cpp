#include <unity.h>
#include "GridClearGame.h"
#include "MockLedController.h"

void setUp(void) {}
void tearDown(void) {}

void test_all_zones_active_at_start() {
    GridClearGame game(4, 5000);
    game.begin(1000);
    TEST_ASSERT_FALSE(game.isGameOver());
    TEST_ASSERT_EQUAL_UINT16(0, game.result().totalHits);
}

void test_hitting_all_zones_completes_before_timeout() {
    GridClearGame game(3, 5000);
    game.begin(0);
    game.onHit(0, 900, 100);
    game.onHit(1, 900, 200);
    game.update(200);
    TEST_ASSERT_FALSE(game.isGameOver());

    game.onHit(2, 900, 300);
    game.update(300);
    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_TRUE(game.result().completed);
    TEST_ASSERT_EQUAL_UINT16(3, game.result().totalHits);
}

void test_time_expires_before_all_zones_cleared() {
    GridClearGame game(4, 1000);
    game.begin(0);
    game.onHit(0, 900, 100);
    game.update(1000);
    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_FALSE(game.result().completed);
    TEST_ASSERT_EQUAL_UINT16(1, game.result().totalHits);
}

void test_hitting_already_cleared_zone_counts_as_extra() {
    GridClearGame game(2, 5000);
    game.begin(0);
    game.onHit(0, 900, 100);
    game.onHit(0, 900, 150); // re-hit an already-cleared zone
    TEST_ASSERT_EQUAL_UINT16(1, game.result().totalHits);
    TEST_ASSERT_EQUAL_UINT16(1, game.result().extraHits);
}

void test_render_shows_active_vs_cleared_colors() {
    GridClearGame game(2, 5000);
    game.begin(0);
    game.onHit(0, 900, 50);
    MockLedController leds;
    game.render(leds);
    TEST_ASSERT_EQUAL_UINT8(0, leds.colors[0].r);
    TEST_ASSERT_TRUE(leds.colors[0].g > 0);  // zone 0 cleared -> green
    TEST_ASSERT_TRUE(leds.colors[1].r > 0);  // zone 1 still live -> red/amber
    TEST_ASSERT_EQUAL_INT(1, leds.showCount);
}

void test_hit_after_game_over_is_ignored() {
    GridClearGame game(1, 1000);
    game.begin(0);
    game.update(1000); // time expires with zone 0 never hit
    TEST_ASSERT_TRUE(game.isGameOver());
    game.onHit(0, 900, 1200);
    TEST_ASSERT_EQUAL_UINT16(0, game.result().totalHits);
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_all_zones_active_at_start);
    RUN_TEST(test_hitting_all_zones_completes_before_timeout);
    RUN_TEST(test_time_expires_before_all_zones_cleared);
    RUN_TEST(test_hitting_already_cleared_zone_counts_as_extra);
    RUN_TEST(test_render_shows_active_vs_cleared_colors);
    RUN_TEST(test_hit_after_game_over_is_ignored);
    return UNITY_END();
}
