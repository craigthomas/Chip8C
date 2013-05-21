/**
 * Copyright (C) 2012 Craig Thomas
 *
 * @file      keyboard.c
 * @brief     Routines for addressing emulator keyboard
 * @author    Craig Thomas
 * @copyright MIT style license - see the LICENSE file for details
 * @copyright @verbinclude LICENSE
 *
 * The emulator needs to define keys from 0-9 and a-f. The emulator CPU expects
 * these values from the keyboard to be returned as their associated hex value.
 * For example, key '0' will have a hex value of 0x0. Similarly, key 'A' will
 * have a hex value of 0xA. The keyboard routines use the SDL (Simple Direct
 * Media Layer) library to read the status of the keyboard. Because we don't
 * want to expose direct SDLKey values to the CPU, there are a number of 
 * routines that convert the SDLKey values to their associated hex values. 
 *
 * The `keyboard_def` variable controls the mapping between the emulator keys
 * 0-9 and a-f to their associated SDLKey values. Change the mapping below if
 * you want to change the emulator keyboard.
 *
 * In addition to the emulator keys, there are two special keys, the 'ESC' and
 * 'F1' keys. Pressing 'F1' will place the emulator into a debug state where 
 * each operation will be displayed on STDOUT. The 'ESC' key will immediately
 * stop the emulator.
 */

/* I N C L U D E S ************************************************************/

#include <malloc.h>
#include "globals.h"

/* L O C A L S ****************************************************************/

/*!
 * Keyboard mapping from SDL keys to key values 
 */
KEYSPEC keyboard_def[] = 
{
   { 0x1, SDLK_4 },
   { 0x2, SDLK_5 },
   { 0x3, SDLK_6 },
   { 0x4, SDLK_r },
   { 0x5, SDLK_t },
   { 0x6, SDLK_y },
   { 0x7, SDLK_f },
   { 0x8, SDLK_g },
   { 0x9, SDLK_h },
   { 0xA, SDLK_v },
   { 0xB, SDLK_b },
   { 0xC, SDLK_7 },
   { 0xD, SDLK_u },
   { 0xE, SDLK_j },
   { 0xF, SDLK_n }
};

/* F U N C T I O N S **********************************************************/

/**
 * Takes a keycode and returns the associated SDLKey mapping. Returns SDLK_END 
 * on no match.
 *
 * @param keycode the keycode to check for
 * @return the SDLKey that is associated with the keycode
 */
SDLKey 
keyboard_keycodetosymbol(int keycode)
{
    int i;
    SDLKey result = SDLK_END;

    for (i = 0; i < KEY_NUMBEROFKEYS; i++) {
        if (keyboard_def[i].keycode == keycode) {
            result = keyboard_def[i].symbol;
        }
    }
    return result;
}

/******************************************************************************/

/**
 * Takes the SDLKey symbol and checks to see if there is an associated keycode. 
 * Returns KEY_NOKEY on no match.
 *
 * @param symbol the SDLKey to check
 * @return the associated keycode, or KEY_NOKEY if no key mapping exists
 */
int 
keyboard_symboltokeycode(SDLKey symbol)
{
    int i;
    int result = KEY_NOKEY;

    for (i = 0; i < KEY_NUMBEROFKEYS; i++) {
        if (keyboard_def[i].symbol == symbol) {
            result = keyboard_def[i].keycode;
        }
    }
    return result;
}

/******************************************************************************/

/**
 * Checks to see whether or not the specified keycode was pressed. Returns 
 * TRUE if the key was pressed, FALSE otherwise.
 *
 * @param keycode the keycode to check
 * @return TRUE if the key was pressed, FALSE otherwise
 */
int 
keyboard_checkforkeypress(int keycode)
{
    int result = FALSE;
    Uint8 *keystates = SDL_GetKeyState(NULL);
    if (keystates[keyboard_keycodetosymbol(keycode)]) {
        result = TRUE;
    }
    return result;
}

/******************************************************************************/

/**
 * Wait and loop until a key is pressed. Returns the keycode of the key that 
 * was pressed, or 0 on failure.
 *
 * @return the keycode value of the key pressed
 */
int 
keyboard_waitgetkeypress(void)
{
    int keycode = KEY_NOKEY;
    while (keycode == KEY_NOKEY) {
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    keycode = keyboard_symboltokeycode(event.key.keysym.sym);
                    break;

                default:
                    break;
            }
        }
        SDL_Delay(20);
    }
    return keycode;
}

/* E N D   O F   F I L E ******************************************************/
