/**
 * Copyright (C) 2012 Craig Thomas
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
SDL_Surface *overlay;          /**< Stores overlay data for the screen        */
SDL_Surface *virtscreen;       /**< Stores the Chip 8 virtual screen          */
int screen_width;              /**< Stores the width of the screen in pixels  */
int screen_height;             /**< Stores the height of the screen in pixels */
int scale_factor;              /**< The scale factor applied to the screen    */
int screen_extended_mode;      /**< Whether the screen is in extended mode    */

/* Colors */
Uint32 COLOR_BLACK;            /**< Black pixel color                         */
Uint32 COLOR_WHITE;            /**< White pixel color                         */
Uint32 COLOR_DGREEN;           /**< Dark green pixel color (for overlay)      */
Uint32 COLOR_LGREEN;           /**< Light green pixel color (for overlay)     */
SDL_Color COLOR_TEXT;          /**< Text color (white)                        */

/* CPU */
chip8regset cpu;               /**< The main emulator CPU                     */
SDL_TimerID cpu_timer;         /**< A CPU tick timer                          */
unsigned long cpu_interrupt;   /**< The CPU interrupt routine                 */
int decrement_timers;          /**< Flags CPU to decrement DELAY and SOUND    */
int op_delay;                  /**< Millisecond delay on the CPU              */

/* Event captures */
SDL_Event event;               /**< Stores SDL events                         */

/* E N D   O F   F I L E ******************************************************/