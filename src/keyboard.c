/**
 * Copyright (C) 2012-2025 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      keyboard.c
 * @brief     Routines for addressing emulator keyboard
 * @author    Craig Thomas
 *
 * The emulator needs to define keys from 0-9 and a-f. The emulator CPU expects
 * these values from the keyboard to be returned as their associated hex value.
 * For example, key '0' will have a hex value of 0x0. Similarly, key 'A' will
 * have a hex value of 0xA. The keyboard routines use the SDL (Simple Direct
 * Media Layer) library to read the status of the keyboard. Because we don't
 * want to expose direct SDL_KeyCode values to the CPU, there are a number of 
 * routines that convert the SDL_KeyCode values to their associated hex values. 
 *
 * The `keyboard_def` variable controls the mapping between the emulator keys
 * 0-9 and a-f to their associated SDL_KeyCode values. Change the mapping below if
 * you want to change the emulator keyboard.
 */

/* I N C L U D E S ************************************************************/

#include <malloc.h>

#ifndef GLOBALS_H_
#include "globals.h"
#endif

/* L O C A L S ****************************************************************/

/*!
 * Keyboard mapping from SDL keys to key values 
 */
SDL_KeyCode keyboard_def[] = 
{
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v
};

/*!
 * The state of the pressed keys on the keyboard
 */
int keyboard_state[] = {
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE, 
    FALSE
};

/* F U N C T I O N S **********************************************************/

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
    if (keycode >= 0 && keycode < KEY_NUMBEROFKEYS) {
        return keyboard_state[keycode];
    }
    return FALSE;
}

/******************************************************************************/

/**
 * If the key pressed is a valid emulator key, will return the key encoding 
 * for that key pressed, or -1 if the key is not a valid emulator key
 * 
 * @param key the SDLKey to check
 * @return the value of the key if it is an emulator key, -1 otherwise
 */
int
keyboard_isemulatorkey(SDL_KeyCode key)
{
    for (int x=0; x < KEY_NUMBEROFKEYS; x++) {
        if (key == keyboard_def[x]) {
            return x;
        }
    }
    return -1;
}

/******************************************************************************/

/**
 * Processes a keypress. Will check to see what key was pressed, and sets the
 * corresponding keypress state in the keyboard matrix to TRUE.
 * 
 * @param key the SDLKey to process
 */
void
keyboard_processkeydown(SDL_KeyCode key) 
{
    for (int x=0; x < KEY_NUMBEROFKEYS; x++) {
        if (key == keyboard_def[x]) {
            keyboard_state[x] = TRUE;
        }
    }
}

/******************************************************************************/

/**
 * Processes a key release. Will check to see what key was released, and sets the
 * corresponding keypress state in the keyboard matrix to FALSE.
 * 
 * @param key the SDLKey to process
 */
void
keyboard_processkeyup(SDL_KeyCode key) 
{
    for (int x=0; x < KEY_NUMBEROFKEYS; x++) {
        if (key == keyboard_def[x]) {
            keyboard_state[x] = FALSE;
        }
    }
}

/* E N D   O F   F I L E ******************************************************/
