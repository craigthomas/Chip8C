/**
 * Copyright (C) 2012-2019 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      screen_test.c
 * @brief     Tests for the screen functions
 * @author    Craig Thomas
 */

/* I N C L U D E S ************************************************************/

#include <CUnit/CUnit.h>
#include "globals.h"

/* F U N C T I O N S **********************************************************/

void 
setup_screen_test(void) 
{
    scale_factor = 1;
    CU_TEST_FATAL(screen_init());
}

void 
teardown_screen_test(void)
{
    screen_destroy();
}

void
test_set_get_pixel(void) 
{
    setup_screen_test();
    draw_pixel(10, 10, 1, 1);
    CU_ASSERT_TRUE(get_pixel(10, 10, 1));
    teardown_screen_test();
}

void
test_set_pixel_color_zero_turns_pixel_off(void) 
{
    setup_screen_test();
    draw_pixel(10, 10, 1, 1);
    draw_pixel(10, 10, 0, 1);
    CU_ASSERT_FALSE(get_pixel(10, 10, 1));
    teardown_screen_test();
}

void
test_screen_blank(void)
{
    int x = 0;
    int y = 0;

    setup_screen_test();
    for (x = 0; x < screen_get_width(); x++) {
        for (y = 0; y < screen_get_height(); y++) {
            draw_pixel(x, y, 1, 1);
        }
    }
    screen_blank(1);
    for (x = 0; x < screen_get_width(); x++) {
        for (y = 0; y < screen_get_height(); y++) {
            CU_ASSERT_FALSE(get_pixel(x, y, 1));
        }
    }
    teardown_screen_test();
}

void
test_screen_get_width_normal(void)
{
    setup_screen_test();
    CU_ASSERT_EQUAL(64, screen_get_width());
    teardown_screen_test();
}

void
test_screen_get_width_extended(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    CU_ASSERT_EQUAL(128, screen_get_width());
    teardown_screen_test();
}

void
test_screen_get_height_normal(void)
{
    setup_screen_test();
    screen_set_normal_mode();
    CU_ASSERT_EQUAL(32, screen_get_height());
    teardown_screen_test();
}

void
test_screen_get_height_extended(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    CU_ASSERT_EQUAL(64, screen_get_height());
    teardown_screen_test();
}

void
test_screen_scroll_right(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(1, 1, TRUE, 1);
    draw_pixel(1, 1, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(1, 1, 1));
    CU_ASSERT_TRUE(get_pixel(1, 1, 2));
    screen_scroll_right(1);
    CU_ASSERT_FALSE(get_pixel(1, 1, 1));
    CU_ASSERT_TRUE(get_pixel(5, 1, 1));
    CU_ASSERT_TRUE(get_pixel(1, 1, 2));
    CU_ASSERT_FALSE(get_pixel(5, 1, 2));
    teardown_screen_test();
}

void
test_screen_scroll_right_bitplane_0_does_nothing(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(0, 1, TRUE, 1);
    draw_pixel(0, 1, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(0, 1, 1));
    CU_ASSERT_TRUE(get_pixel(0, 1, 2));
    screen_scroll_right(0);
    CU_ASSERT_TRUE(get_pixel(0, 1, 1));
    CU_ASSERT_FALSE(get_pixel(1, 1, 1));
    CU_ASSERT_FALSE(get_pixel(2, 1, 1));
    CU_ASSERT_FALSE(get_pixel(3, 1, 1));
    CU_ASSERT_FALSE(get_pixel(4, 1, 1));
    CU_ASSERT_TRUE(get_pixel(0, 1, 2));
    CU_ASSERT_FALSE(get_pixel(1, 1, 2));
    CU_ASSERT_FALSE(get_pixel(2, 1, 2));
    CU_ASSERT_FALSE(get_pixel(3, 1, 2));
    CU_ASSERT_FALSE(get_pixel(4, 1, 2));
    teardown_screen_test();
}

void
test_screen_scroll_right_bitplane_3(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(0, 1, TRUE, 1);
    draw_pixel(0, 1, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(0, 1, 1));
    CU_ASSERT_TRUE(get_pixel(0, 1, 2));
    screen_scroll_right(3);
    CU_ASSERT_FALSE(get_pixel(0, 1, 1));
    CU_ASSERT_FALSE(get_pixel(1, 1, 1));
    CU_ASSERT_FALSE(get_pixel(2, 1, 1));
    CU_ASSERT_FALSE(get_pixel(3, 1, 1));
    CU_ASSERT_TRUE(get_pixel(4, 1, 1));
    CU_ASSERT_FALSE(get_pixel(0, 1, 2));
    CU_ASSERT_FALSE(get_pixel(1, 1, 2));
    CU_ASSERT_FALSE(get_pixel(2, 1, 2));
    CU_ASSERT_FALSE(get_pixel(3, 1, 2));
    CU_ASSERT_TRUE(get_pixel(4, 1, 2));
    teardown_screen_test();
}

void
test_screen_scroll_left(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(5, 1, 1, 1);
    CU_ASSERT_TRUE(get_pixel(5, 1, 1));
    screen_scroll_left(1);
    CU_ASSERT_FALSE(get_pixel(5, 1, 1));
    CU_ASSERT_TRUE(get_pixel(1, 1, 1));
    teardown_screen_test();
}

void
test_screen_scroll_left_bitplane_0_does_nothing(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(63, 0, TRUE, 1);
    draw_pixel(63, 0, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(63, 0, 1));
    CU_ASSERT_TRUE(get_pixel(63, 0, 2));
    screen_scroll_left(0);
    CU_ASSERT_TRUE(get_pixel(63, 0, 1));
    CU_ASSERT_FALSE(get_pixel(62, 0, 1));
    CU_ASSERT_FALSE(get_pixel(61, 0, 1));
    CU_ASSERT_FALSE(get_pixel(60, 0, 1));
    CU_ASSERT_FALSE(get_pixel(59, 0, 1));
    CU_ASSERT_TRUE(get_pixel(63, 0, 2));
    CU_ASSERT_FALSE(get_pixel(62, 0, 2));
    CU_ASSERT_FALSE(get_pixel(61, 0, 2));
    CU_ASSERT_FALSE(get_pixel(60, 0, 2));
    CU_ASSERT_FALSE(get_pixel(59, 0, 2));
    teardown_screen_test();
}

void
test_screen_scroll_left_bitplane_3(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(63, 0, TRUE, 1);
    draw_pixel(63, 0, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(63, 0, 1));
    CU_ASSERT_TRUE(get_pixel(63, 0, 2));
    screen_scroll_left(3);
    CU_ASSERT_FALSE(get_pixel(63, 0, 1));
    CU_ASSERT_FALSE(get_pixel(62, 0, 1));
    CU_ASSERT_FALSE(get_pixel(61, 0, 1));
    CU_ASSERT_FALSE(get_pixel(60, 0, 1));
    CU_ASSERT_TRUE(get_pixel(59, 0, 1));
    CU_ASSERT_FALSE(get_pixel(63, 0, 2));
    CU_ASSERT_FALSE(get_pixel(62, 0, 2));
    CU_ASSERT_FALSE(get_pixel(61, 0, 2));
    CU_ASSERT_FALSE(get_pixel(60, 0, 2));
    CU_ASSERT_TRUE(get_pixel(59, 0, 2));
    teardown_screen_test();
}

void
test_screen_scroll_down(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(1, 5, 1, 1);
    CU_ASSERT_TRUE(get_pixel(1, 5, 1));
    screen_scroll_down(4, 1);
    CU_ASSERT_FALSE(get_pixel(1, 5, 1));
    CU_ASSERT_TRUE(get_pixel(1, 9, 1));
    teardown_screen_test();
}

void
test_screen_scroll_down_bitplane_0_does_nothing(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(0, 0, TRUE, 1);
    draw_pixel(0, 0, TRUE, 2);
    screen_scroll_down(4, 0);
    CU_ASSERT_TRUE(get_pixel(0, 0, 1));
    CU_ASSERT_TRUE(get_pixel(0, 0, 2));
    teardown_screen_test();
}

void
test_screen_scroll_down_bitplane_1_both_pixels_active(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(0, 0, TRUE, 1);
    draw_pixel(0, 0, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(0, 0, 1));
    CU_ASSERT_TRUE(get_pixel(0, 0, 2));
    screen_scroll_down(1, 1);
    CU_ASSERT_FALSE(get_pixel(0, 0, 1));
    CU_ASSERT_TRUE(get_pixel(0, 0, 2));
    CU_ASSERT_TRUE(get_pixel(0, 1, 1));
    CU_ASSERT_FALSE(get_pixel(0, 1, 2));
    teardown_screen_test();
}

void
test_screen_scroll_down_bitplane_3_both_pixels_active(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(0, 0, TRUE, 1);
    draw_pixel(0, 0, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(0, 0, 1));
    CU_ASSERT_TRUE(get_pixel(0, 0, 2));
    screen_scroll_down(1, 3);
    CU_ASSERT_FALSE(get_pixel(0, 0, 1));
    CU_ASSERT_FALSE(get_pixel(0, 0, 2));
    CU_ASSERT_TRUE(get_pixel(0, 1, 1));
    CU_ASSERT_TRUE(get_pixel(0, 1, 2));
    teardown_screen_test();
}

void
test_screen_scroll_up(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(1, 5, 1, 1);
    CU_ASSERT_TRUE(get_pixel(1, 5, 1));
    screen_scroll_up(4, 1);
    CU_ASSERT_FALSE(get_pixel(1, 5, 1));
    CU_ASSERT_TRUE(get_pixel(1, 1, 1));
    teardown_screen_test();
}

void
test_screen_scroll_up_bitplane_0_does_nothing(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(1, 5, TRUE, 1);
    draw_pixel(1, 5, TRUE, 2);
    screen_scroll_up(4, 0);
    CU_ASSERT_TRUE(get_pixel(1, 5, 1));
    CU_ASSERT_TRUE(get_pixel(1, 5, 2));
    teardown_screen_test();
}

void
test_screen_scroll_up_bitplane_1_both_pixels_active(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(1, 5, TRUE, 1);
    draw_pixel(1, 5, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(1, 5, 1));
    CU_ASSERT_TRUE(get_pixel(1, 5, 2));
    screen_scroll_up(4, 1);
    CU_ASSERT_FALSE(get_pixel(1, 5, 1));
    CU_ASSERT_TRUE(get_pixel(1, 5, 2));
    CU_ASSERT_TRUE(get_pixel(1, 1, 1));
    CU_ASSERT_FALSE(get_pixel(1, 1, 2));
    teardown_screen_test();
}

void
test_screen_scroll_up_bitplane_3_both_pixels_active(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    draw_pixel(1, 5, TRUE, 1);
    draw_pixel(1, 5, TRUE, 2);
    CU_ASSERT_TRUE(get_pixel(1, 5, 1));
    CU_ASSERT_TRUE(get_pixel(1, 5, 2));
    screen_scroll_up(4, 3);
    CU_ASSERT_FALSE(get_pixel(1, 5, 1));
    CU_ASSERT_FALSE(get_pixel(1, 5, 2));
    CU_ASSERT_TRUE(get_pixel(1, 1, 1));
    CU_ASSERT_TRUE(get_pixel(1, 1, 2));
    teardown_screen_test();
}

void 
test_screen_get_mode_scale_normal(void)
{
    setup_screen_test();
    screen_set_normal_mode();
    CU_ASSERT_EQUAL(2, screen_get_mode_scale());
}

void 
test_screen_get_mode_scale_extended(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    CU_ASSERT_EQUAL(1, screen_get_mode_scale());
}

void
test_screen_is_mode_extended_correct(void)
{
    setup_screen_test();
    screen_set_extended_mode();
    CU_ASSERT_TRUE(screen_is_extended_mode());

    screen_set_normal_mode();
    CU_ASSERT_FALSE(screen_is_extended_mode());
}

/* E N D   O F   F I L E *****************************************************/