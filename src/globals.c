/**
 * Copyright (C) 2012-2025 Craig Thomas
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
SDL_Window *window;            /**< Stores the main screen SDL structure      */
SDL_Surface *surface;          /**< Stores the Chip 8 virtual screen          */
SDL_Renderer *renderer;        /**< Stores the screen renderer                */
SDL_Texture *texture;          /**< The SDL texture to render                 */
int scale;                     /**< The scale factor applied to the screen    */
int screen_mode;               /**< Whether the screen is in extended mode    */
int scale_factor;              /**< Stores the current scale factor           */

/* Colors */
Uint32 COLOR_0;                /**< Bitplane 0 color                          */
Uint32 COLOR_1;                /**< Bitplane 1 color                          */
Uint32 COLOR_2;                /**< Bitplane 2 color                          */
Uint32 COLOR_3;                /**< Bitplane 3 color                          */

/* CPU */
chip8regset cpu;               /**< The main emulator CPU                     */
SDL_TimerID cpu_timer;         /**< A CPU tick timer                          */
unsigned long cpu_interrupt;   /**< The CPU interrupt routine                 */
int decrement_timers;          /**< Flags CPU to decrement DELAY and SOUND    */
int op_delay;                  /**< Millisecond delay on the CPU              */
int awaiting_keypress;         /**< Whether to wait for a keypress event      */
float playback_rate;           /**< The playback rate for audio               */
int pitch;                     /**< The pitch for the current audio sample    */
int bitplane;                  /**< Sets the current drawing plane            */
byte audio_pattern_buffer[16]; /**< Stores the audio pattern buffer           */
Mix_Chunk audio_chunk;         /**< The currently created audio chunk         */
int audio_playing;             /**< Stores whether audio is playing           */
int tick_counter;              /**< Stores how many ticks have been executed  */

/* Event captures */
SDL_Event event;               /**< Stores SDL events                         */

/* Emulator flags */
int jump_quirks;               /**< Stores whether jump quirks are turned on  */
int shift_quirks;              /**< Stores whether shift quirks are turned on */
int index_quirks;              /**< Stores whether index quirks are turned on */
int logic_quirks;              /**< Stores whether logic quirks are turned on */
int clip_quirks;               /**< Stores whether clip quirks are turned on  */
int max_ticks;                 /**< Stores how many ticks per second allowed  */


/* E N D   O F   F I L E ******************************************************/