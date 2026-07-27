#include <unity.h>
#include "KnockoutGame.h"
#include "MockLedController.h"

void setUp(void) {}
void tearDown(void) {}

void test_all_zones_active_at_start() {
    KnockoutGame game(4, 90000);
    game.begin(1000);
    TEST_ASSERT_FALSE(game.isGameOver());
    TEST_ASSERT_EQUAL_UINT16(0, game.result().totalHits);
}

void test_hitting_all_zones_completes_before_timeout() {
    KnockoutGame game(3, 90000);
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

void test_time_expires_before_all_zones_cleared_scores_knockout_count() {
    KnockoutGame game(12, 90000);
    game.begin(0);
    for (uint8_t z = 0; z < 9; ++z) game.onHit(z, 900, 1000 * (z + 1));
    game.update(90000); // ninety seconds up, period
    TEST_ASSERT_TRUE(game.isGameOver());
    TEST_ASSERT_FALSE(game.result().completed);
    TEST_ASSERT_EQUAL_UINT16(9, game.result().totalHits); // score = zones knocked out
}

void test_hitting_already_cleared_zone_counts_as_extra() {
    KnockoutGame game(2, 90000);
    game.begin(0);
    game.onHit(0, 900, 100);
    game.onHit(0, 900, 150); // re-hit an already-cleared zone
    TEST_ASSERT_EQUAL_UINT16(1, game.result().totalHits);
    TEST_ASSERT_EQUAL_UINT16(1, game.result().extraHits);
}

void test_render_shows_active_vs_cleared_colors() {
    KnockoutGame game(2, 90000);
    game.begin(0);
    game.onHit(0, 900, 50);
    MockLedController leds;
    game.render(leds);
    TEST_ASSERT_EQUAL_UINT8(0, leds.colors[0].r);
    TEST_ASSERT_TRUE(leds.colors[0].g > 0);  // zone 0 knocked out -> green
    TEST_ASSERT_TRUE(leds.colors[1].r > 0);  // zone 1 still live -> red/amber
    TEST_ASSERT_EQUAL_INT(1, leds.showCount);
}

void test_hit_after_game_over_is_ignored() {
    KnockoutGame game(1, 90000);
    game.begin(0);
    game.update(90000); // time expires with zone 0 never hit
    TEST_ASSERT_TRUE(game.isGameOver());
    game.onHit(0, 900, 91000);
    TEST_ASSERT_EQUAL_UINT16(0, game.result().totalHits);
}

void test_full_clear_records_best_time() {
    KnockoutGame game(2, 90000);
    TEST_ASSERT_EQUAL_UINT32(0, game.bestClearTimeMs()); // no record yet

    game.begin(0);
    game.onHit(0, 900, 10000);
    game.onHit(1, 900, 12000);
    game.update(12000);

    TEST_ASSERT_TRUE(game.result().completed);
    TEST_ASSERT_EQUAL_UINT32(12000, game.bestClearTimeMs());
    TEST_ASSERT_EQUAL_UINT32(12000, game.result().bestTimeMs);
}

void test_slower_clear_does_not_overwrite_a_faster_record() {
    KnockoutGame game(2, 90000);

    game.begin(0);
    game.onHit(0, 900, 5000);
    game.onHit(1, 900, 8000);
    game.update(8000);
    TEST_ASSERT_EQUAL_UINT32(8000, game.bestClearTimeMs());

    game.begin(100000); // second round starts later, best time persists
    game.onHit(0, 900, 100000 + 20000);
    game.onHit(1, 900, 100000 + 25000);
    game.update(100000 + 25000);

    TEST_ASSERT_EQUAL_UINT32(8000, game.bestClearTimeMs()); // still the faster round
}

void test_faster_clear_overwrites_the_record() {
    KnockoutGame game(2, 90000);

    game.begin(0);
    game.onHit(0, 900, 20000);
    game.onHit(1, 900, 25000);
    game.update(25000);
    TEST_ASSERT_EQUAL_UINT32(25000, game.bestClearTimeMs());

    game.begin(100000);
    game.onHit(0, 900, 100000 + 4000);
    game.onHit(1, 900, 100000 + 6000);
    game.update(100000 + 6000);

    TEST_ASSERT_EQUAL_UINT32(6000, game.bestClearTimeMs());
}

void test_timeout_round_does_not_touch_best_time() {
    KnockoutGame game(2, 90000);

    game.begin(0);
    game.onHit(0, 900, 5000);
    game.onHit(1, 900, 7000);
    game.update(7000);
    TEST_ASSERT_EQUAL_UINT32(7000, game.bestClearTimeMs());

    game.begin(100000);
    game.onHit(0, 900, 100000 + 1000); // only clears one zone
    game.update(100000 + 90000);       // times out
    TEST_ASSERT_FALSE(game.result().completed);
    TEST_ASSERT_EQUAL_UINT32(7000, game.bestClearTimeMs()); // unchanged
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_all_zones_active_at_start);
    RUN_TEST(test_hitting_all_zones_completes_before_timeout);
    RUN_TEST(test_time_expires_before_all_zones_cleared_scores_knockout_count);
    RUN_TEST(test_hitting_already_cleared_zone_counts_as_extra);
    RUN_TEST(test_render_shows_active_vs_cleared_colors);
    RUN_TEST(test_hit_after_game_over_is_ignored);
    RUN_TEST(test_full_clear_records_best_time);
    RUN_TEST(test_slower_clear_does_not_overwrite_a_faster_record);
    RUN_TEST(test_faster_clear_overwrites_the_record);
    RUN_TEST(test_timeout_round_does_not_touch_best_time);
    return UNITY_END();
}
