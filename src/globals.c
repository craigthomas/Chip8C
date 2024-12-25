/**
 * Copyright (C) 2012-2024 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      globals.h
 * @brief     Global variables for the emulator
 * @author    Craig Thomas
 *
 * This file contains several function definitions and type declarations for
 * the emulator. 
 */

/* I N C L U D E S ***********************************************************/

#include "globals.h"

/* G L O B A L S *************************************************************/

/* Memory */
byte *memory;                  /**< Pointer to emulator memory region         */

/* Screen */
SDL_Surface *screen;           /**< Stores the main screen SDL structure      */
SDL_Surface *virtscreen;       /**< Stores the Chip 8 virtual screen          */
int scale;                     /**< The scale factor applied to the screen    */
int screen_mode;               /**< Whether the screen is in extended mode    */
int scale_factor;              /**< Stores the current scale factor           */

/* Colors */
Uint32 COLOR_BLACK;            /**< Black pixel color                         */
Uint32 COLOR_WHITE;            /**< White pixel color                         */

/* CPU */
chip8regset cpu;               /**< The main emulator CPU                     */
SDL_TimerID cpu_timer;         /**< A CPU tick timer                          */
unsigned long cpu_interrupt;   /**< The CPU interrupt routine                 */
int decrement_timers;          /**< Flags CPU to decrement DELAY and SOUND    */
int op_delay;                  /**< Millisecond delay on the CPU              */
int awaiting_keypress;         /**< Whether to wait for a keypress event      */

/* Event captures */
SDL_Event event;               /**< Stores SDL events                         */

/* E N D   O F   F I L E ******************************************************/