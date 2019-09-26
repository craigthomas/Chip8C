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

/* L O C A L S ****************************************************************/

word tword;
word address;

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

/* E N D   O F   F I L E *****************************************************/