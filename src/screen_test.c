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
    screen_draw(10, 10, 1);
    CU_ASSERT_TRUE(screen_getpixel(10, 10));
    teardown_screen_test();
}

void
test_set_pixel_color_zero_turns_pixel_off(void) 
{
    setup_screen_test();
    screen_draw(10, 10, 1);
    screen_draw(10, 10, 0);
    CU_ASSERT_FALSE(screen_getpixel(10, 10));
    teardown_screen_test();
}

void
test_screen_blank(void)
{
    int x = 0;
    int y = 0;

    setup_screen_test();
    for (x = 0; x < SCREEN_WIDTH; x++) {
        for (y = 0; y < SCREEN_HEIGHT; y++) {
            screen_draw(x, y, 1);
        }
    }
    screen_blank();
    for (x = 0; x < SCREEN_WIDTH; x++) {
        for (y = 0; y < SCREEN_HEIGHT; y++) {
            CU_ASSERT_FALSE(screen_getpixel(x, y));
        }
    }
    teardown_screen_test();
}

void
test_screen_get_width_normal(void)
{
    setup_screen_test();
    screen_extended_mode = FALSE;
    CU_ASSERT_EQUAL(SCREEN_WIDTH, screen_get_width());
    teardown_screen_test();
}

void
test_screen_get_width_extended(void)
{
    setup_screen_test();
    screen_extended_mode = TRUE;
    CU_ASSERT_EQUAL(SCREEN_EXT_WIDTH, screen_get_width());
    teardown_screen_test();
}

void
test_screen_get_height_normal(void)
{
    setup_screen_test();
    screen_extended_mode = FALSE;
    CU_ASSERT_EQUAL(SCREEN_HEIGHT, screen_get_height());
    teardown_screen_test();
}

void
test_screen_get_height_extended(void)
{
    setup_screen_test();
    screen_extended_mode = TRUE;
    CU_ASSERT_EQUAL(SCREEN_EXT_HEIGHT, screen_get_height());
    teardown_screen_test();
}

void
test_screen_scroll_right(void)
{
    setup_screen_test();
    screen_set_extended();
    screen_draw(1, 1, 1);
    CU_ASSERT_TRUE(screen_getpixel(1, 1));
    screen_scroll_right();
    CU_ASSERT_FALSE(screen_getpixel(1, 1));
    CU_ASSERT_TRUE(screen_getpixel(5, 1));
    teardown_screen_test();
}

void
test_screen_scroll_left(void)
{
    setup_screen_test();
    screen_set_extended();
    screen_draw(5, 1, 1);
    CU_ASSERT_TRUE(screen_getpixel(5, 1));
    screen_scroll_left();
    CU_ASSERT_FALSE(screen_getpixel(5, 1));
    CU_ASSERT_TRUE(screen_getpixel(1, 1));
    teardown_screen_test();
}

void
test_screen_scroll_down(void)
{
    setup_screen_test();
    screen_set_extended();
    screen_draw(1, 5, 1);
    CU_ASSERT_TRUE(screen_getpixel(1, 5));
    screen_scroll_down(4);
    CU_ASSERT_FALSE(screen_getpixel(1, 5));
    CU_ASSERT_TRUE(screen_getpixel(1, 9));
    teardown_screen_test();
}

/* E N D   O F   F I L E *****************************************************/