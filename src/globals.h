/**
 * Copyright (C) 2012 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      globals.h
 * @brief     Global parameters and variables for the emulator
 * @author    Craig Thomas
 *
 * This file contains several function definitions and type declarations for
 * the emulator. 
 */

#ifndef GLOBALS_H_ 
#define GLOBALS_H_

/* I N C L U D E S ************************************************************/

#include <SDL.h>

/* D E F I N E S **************************************************************/

/* Memory */
#define MEM_4K         0x1000    /**< Defines a 4K memory size                */
#define SP_START       0x52      /**< Defines the start of the system stack   */
#define ROM_DEFAULT    0x200	 /**< Defines the default ROM load point        */

/* Screen */
#define SCREEN_HEIGHT      32    /**< Default screen height                   */
#define SCREEN_WIDTH       64    /**< Default screen width                    */
#define SCREEN_EXT_HEIGHT  64    /**< Extended screen height                  */
#define SCREEN_EXT_WIDTH   128   /**< Extended screen width                   */
#define SCREEN_DEPTH       32    /**< Colour depth in BPP                     */
#define SCALE_FACTOR       5     /**< Scaling for the window size             */
#define PIXEL_COLOR        250   /**< Color to use for drawing pixels         */
#define SCREEN_VERTREFRESH 60    /**< Sets the vertical refresh (in Hz)       */

/* CPU */
#define CPU_RUNNING    1         /**< Continues CPU execution                 */
#define CPU_PAUSED     2         /**< Pauses the CPU                          */
#define CPU_DEBUG      3         /**< Sets the debugging mode                 */
#define CPU_TRACE      4         /**< Sets trace without debug                */
#define CPU_STEP       5         /**< Steps to the next instruction (debug)   */
#define CPU_STOP       0         /**< Halts the CPU and quits                 */
#define CPU_OPTIME     1000      /**< CPU tick length (in nanoseconds)        */
#define CPU_PC_START   0x200     /**< The start address of the PC             */

/* Keyboard */
#define KEY_BUFFERSIZE   8    /**< Number of keys to store in the buffer      */
#define KEY_DEBUG        1    /**< Only accept debug keystrokes               */
#define KEY_NORMAL       0    /**< Accept normal keystrokes                   */
#define KEY_NUMBEROFKEYS 15   /**< Defines the number of keys on the keyboard */
#define KEY_NOKEY        -999 /**< Sets the no keypress value                 */

/* Keyboard special keys */
#define QUIT_KEY   SDLK_ESCAPE /**< Quits the emulator                        */
#define DEBUG_KEY  SDLK_z      /**< Puts emulator into debug mode             */
#define TRACE_KEY  SDLK_x      /**< Puts emulator into trace mode             */
#define NORMAL_KEY SDLK_c      /**< Returns emulator to normal running mode   */
#define STEP_KEY   SDLK_n      /**< Runs next instruction (in debug mode)     */

/* Other generic definitions */
#define TRUE          1
#define FALSE         0
#define MAXSTRSIZE    200     /**< Maximum string size for buffers            */

/* T Y P E D E F S ************************************************************/

typedef unsigned char byte;   /**< A single byte of memory                    */

/**
 * A word is actually a combination of two bytes. By using a union, you can
 * access either the low or high byte of the word as a separate entity.
 */
typedef union {
   unsigned short int WORD;
   struct {
      #ifdef WORDS_BIGENDIAN
         byte high, low;
      #else
         byte low, high;
      #endif
   } BYTE;
} word;

/**
 * Maps an SDLKey symbol to a keycode. 
 */
typedef struct {
   int keycode;        /**< The Chip 8 keycode value of the key               */
   SDLKey symbol;      /**< The corresponding SDL symbol value of the key     */
} KEYSPEC;

/**
 * The chip8regset structure represents a Chip 8 cpu. Each of the major 
 * components of the CPU is represented below, along with the current operand
 * and a description of the operation currently being executed.
 */
typedef struct {
    byte v[0x10];      /**< V registers                                       */
    word i;            /**< Index register                                    */
    word pc;           /**< Program Counter register                          */
    word oldpc;        /**< The last PC address                               */
    word sp;           /**< Stack Pointer register                            */
    byte dt;           /**< Delay Timer register                              */
    byte st;           /**< Sound Timer register                              */
    word operand;      /**< The current operand                               */
    char *opdesc;      /**< A string representation of the current opcode     */
    int state;         /**< The current state of the CPU                      */
    byte rpl[0x10];    /**< RPL register storage                              */
} chip8regset;

/* G L O B A L S **************************************************************/

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

/* P R O T O T Y P E S ********************************************************/

/* cpu.c */
void cpu_reset(void);
int cpu_timerinit(void);
void cpu_execute(void);
void cpu_execute_single(void);

/* keyboard.c */
int keyboard_waitgetkeypress(void);
int keyboard_checkforkeypress(int keycode);

/* memory.c */
int memory_init(int memorysize);
void memory_destroy(void);

/**
 * Attempts to read one byte of memory at the requested address. Returns the 
 * byte read from memory.
 *
 * @param address the address in memory to read from
 * @return the value read from the specified memory location
 */
static inline byte 
memory_read(register int address) 
{
   return memory[address];
}

/*****************************************************************************/

/**
 * Attempts to write one byte of information to the requested address.
 *
 * @param address the address in memory to write to
 * @param value the value to write to the memory location
 */
static inline void 
memory_write(register word address, register byte value) 
{
   memory[address.WORD] = value;
}

/*****************************************************************************/

/**
 * Attempts to write one word of information to the requested address.
 *
 * @param address the address in memory to write to
 * @param value the value to write to the memory locations
 */
static inline void 
memory_write_word(register word address, register word value) 
{
   memory[address.WORD] = value.BYTE.high;
   memory[address.WORD + 1] = value.BYTE.low;
}

/*****************************************************************************/

/* screen.c */
int screen_init(void);
void screen_clear(SDL_Surface *surface, Uint32 color); 
void screen_blank(void);
int screen_getpixel(int x, int y);
void screen_draw(int x, int y, int color);
void screen_refresh(int overlay_on);
void screen_destroy(void);
void screen_set_extended(void);
void screen_disable_extended(void);
void screen_scroll_left(void);
void screen_scroll_right(void);
void screen_scroll_down(int num_pixels);
int screen_get_height(void);
int screen_get_width(void);

/* keyboard.c */
SDLKey keyboard_keycodetosymbol(int keycode);
int keyboard_symboltokeycode(SDLKey symbol);

/* cpu_test.c */
void test_jump_to_address(void);
void test_jump_to_subroutine(void);
void test_skip_if_register_equal_value(void);
void test_skip_if_register_not_equal_value(void);
void test_skip_if_register_equal_register(void);
void test_skip_if_register_not_equal_register(void);
void test_move_value_to_register(void);
void test_add_value_to_register(void);
void test_load_register_to_register(void);
void test_logical_or(void);
void test_logical_and(void);
void test_logical_xor(void);
void test_add_register_to_register(void);
void test_sub_register_from_register(void);
void test_shift_register_right(void);
void test_sub_register_from_register_source_from_target(void);
void test_shift_register_left(void);
void test_load_index(void);
void test_jump_index_plus_value(void);
void test_generate_random(void);
void test_load_delay_into_target(void);
void test_load_source_into_delay(void);
void test_load_source_into_sound(void);
void test_add_index(void);
void test_load_sprite_index(void);
void test_bcd(void);
void test_store_registers(void);
void test_load_registers(void);
void test_store_registers_in_rpl(void);
void test_read_registers_from_rpl(void);
void test_return_from_subroutine(void);
void test_exit_interpreter(void);
void test_cpu_scroll_left(void);
void test_cpu_scroll_right(void);
void test_cpu_scroll_down(void);
void test_cpu_screen_blank(void);
void test_cpu_enable_extended_mode(void);
void test_cpu_disable_extended_mode(void);

/* screen_test.c */
void test_set_get_pixel(void);
void test_set_pixel_color_zero_turns_pixel_off(void);
void test_screen_blank(void);
void test_screen_get_width_normal(void);
void test_screen_get_width_extended(void);
void test_screen_get_height_normal(void);
void test_screen_get_height_extended(void);
void test_screen_scroll_right(void);
void test_screen_scroll_left(void);
void test_screen_scroll_down(void);

/* keyboard_test.c */
void test_keycodetosymbol_returns_end_with_invalid_keycode(void);
void test_keycodetosymbol_returns_correct_keycodes(void);
void test_keyboard_symboltokeycode_returns_nokey_on_invalid_symbol(void);
void test_keyboard_symboltokeycode_returns_correct_keycodes(void);
void test_keyboard_checkforkeypress_returns_false_on_no_keypress(void);

#endif

/* E N D   O F   F I L E ******************************************************/
