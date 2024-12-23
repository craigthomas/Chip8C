/**
 * Copyright (C) 2012-2024 Craig Thomas
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
test_keyboard_checkforkeypress_returns_false_on_no_keypress(void)
{
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x0));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x1));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x2));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x3));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x4));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x5));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x6));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x7));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x8));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x9));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xA));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xB));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xC));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xD));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xE));
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xF));
}

void
test_keyboard_process_keydown(void) 
{
    keyboard_processkeydown(SDLK_x);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x0));
    keyboard_processkeydown(SDLK_1);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x1));
    keyboard_processkeydown(SDLK_2);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x2));
    keyboard_processkeydown(SDLK_3);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x3));
    keyboard_processkeydown(SDLK_q);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x4));
    keyboard_processkeydown(SDLK_w);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x5));
    keyboard_processkeydown(SDLK_e);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x6));
    keyboard_processkeydown(SDLK_a);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x7));
    keyboard_processkeydown(SDLK_s);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x8));
    keyboard_processkeydown(SDLK_d);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0x9));
    keyboard_processkeydown(SDLK_z);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0xA));
    keyboard_processkeydown(SDLK_c);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0xB));
    keyboard_processkeydown(SDLK_4);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0xC));
    keyboard_processkeydown(SDLK_r);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0xD));
    keyboard_processkeydown(SDLK_f);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0xE));
    keyboard_processkeydown(SDLK_v);
    CU_ASSERT_TRUE(keyboard_checkforkeypress(0xF));
}

void
test_keyboard_process_keyup(void) 
{
    keyboard_processkeydown(SDLK_x);
    keyboard_processkeyup(SDLK_x);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x0));
    keyboard_processkeydown(SDLK_1);
    keyboard_processkeyup(SDLK_1);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x1));
    keyboard_processkeydown(SDLK_2);
    keyboard_processkeyup(SDLK_2);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x2));
    keyboard_processkeydown(SDLK_3);
    keyboard_processkeyup(SDLK_3);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x3));
    keyboard_processkeydown(SDLK_q);
    keyboard_processkeyup(SDLK_q);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x4));
    keyboard_processkeydown(SDLK_w);
    keyboard_processkeyup(SDLK_w);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x5));
    keyboard_processkeydown(SDLK_e);
    keyboard_processkeyup(SDLK_e);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x6));
    keyboard_processkeydown(SDLK_a);
    keyboard_processkeyup(SDLK_a);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x7));
    keyboard_processkeydown(SDLK_s);
    keyboard_processkeyup(SDLK_s);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x8));
    keyboard_processkeydown(SDLK_d);
    keyboard_processkeyup(SDLK_d);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x9));
    keyboard_processkeydown(SDLK_z);
    keyboard_processkeyup(SDLK_z);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xA));
    keyboard_processkeydown(SDLK_c);
    keyboard_processkeyup(SDLK_c);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xB));
    keyboard_processkeydown(SDLK_4);
    keyboard_processkeyup(SDLK_4);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xC));
    keyboard_processkeydown(SDLK_r);
    keyboard_processkeyup(SDLK_r);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xD));
    keyboard_processkeydown(SDLK_f);
    keyboard_processkeyup(SDLK_f);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xE));
    keyboard_processkeydown(SDLK_v);
    keyboard_processkeyup(SDLK_v);
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0xF));
}

void
test_keyboard_isemulatorkey(void)
{
    CU_ASSERT_EQUAL(-1, keyboard_isemulatorkey(SDLK_k));
    CU_ASSERT_EQUAL(1, keyboard_isemulatorkey(SDLK_1));
}

/* E N D   O F   F I L E *****************************************************/