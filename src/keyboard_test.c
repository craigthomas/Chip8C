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
test_keycodetosymbol_returns_end_with_invalid_keycode(void) 
{
    SDLKey result = keyboard_keycodetosymbol(0x10);
    CU_ASSERT_EQUAL(SDLK_END, result);
}

void
test_keycodetosymbol_returns_correct_keycodes(void) 
{
    CU_ASSERT_EQUAL(SDLK_4, keyboard_keycodetosymbol(0x1));
    CU_ASSERT_EQUAL(SDLK_5, keyboard_keycodetosymbol(0x2));
    CU_ASSERT_EQUAL(SDLK_6, keyboard_keycodetosymbol(0x3));
    CU_ASSERT_EQUAL(SDLK_r, keyboard_keycodetosymbol(0x4));
    CU_ASSERT_EQUAL(SDLK_t, keyboard_keycodetosymbol(0x5));
    CU_ASSERT_EQUAL(SDLK_y, keyboard_keycodetosymbol(0x6));
    CU_ASSERT_EQUAL(SDLK_f, keyboard_keycodetosymbol(0x7));
    CU_ASSERT_EQUAL(SDLK_g, keyboard_keycodetosymbol(0x8));
    CU_ASSERT_EQUAL(SDLK_h, keyboard_keycodetosymbol(0x9));
    CU_ASSERT_EQUAL(SDLK_v, keyboard_keycodetosymbol(0xA));
    CU_ASSERT_EQUAL(SDLK_b, keyboard_keycodetosymbol(0xB));
    CU_ASSERT_EQUAL(SDLK_7, keyboard_keycodetosymbol(0xC));
    CU_ASSERT_EQUAL(SDLK_u, keyboard_keycodetosymbol(0xD));
    CU_ASSERT_EQUAL(SDLK_j, keyboard_keycodetosymbol(0xE));
    CU_ASSERT_EQUAL(SDLK_n, keyboard_keycodetosymbol(0xF));
}

void 
test_keyboard_symboltokeycode_returns_nokey_on_invalid_symbol(void)
{
    int result = keyboard_symboltokeycode(SDLK_z);
    CU_ASSERT_EQUAL(KEY_NOKEY, result);
}

void 
test_keyboard_symboltokeycode_returns_correct_keycodes(void)
{
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_4), 0x1);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_5), 0x2);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_6), 0x3);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_r), 0x4);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_t), 0x5);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_y), 0x6);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_f), 0x7);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_g), 0x8);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_h), 0x9);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_v), 0xA);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_b), 0xB);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_7), 0xC);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_u), 0xD);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_j), 0xE);
    CU_ASSERT_EQUAL(keyboard_symboltokeycode(SDLK_n), 0xF);
}

void 
test_keyboard_checkforkeypress_returns_false_on_no_keypress(void)
{
    CU_ASSERT_FALSE(keyboard_checkforkeypress(0x1));
}

/* E N D   O F   F I L E *****************************************************/