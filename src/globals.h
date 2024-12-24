/**
 * Copyright (C) 2012-2024 Craig Thomas
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
#define MEM_SIZE       0x10000   /**< Defines a 64K memory size               */
#define SP_START       0x52      /**< Defines the start of the system stack   */
#define ROM_DEFAULT    0x200	   /**< Defines the default ROM load point      */

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
#define CPU_STOP       0         /**< Halts the CPU and quits                 */
#define CPU_OPTIME     1000      /**< CPU tick length (in nanoseconds)        */
#define CPU_PC_START   0x200     /**< The start address of the PC             */

/* Keyboard */
#define KEY_NUMBEROFKEYS 16   /**< Defines the number of keys on the keyboard */

/* Keyboard special keys */
#define QUIT_KEY   SDLK_ESCAPE /**< Quits the emulator                        */

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
extern byte *memory;                  /**< Pointer to emulator memory region         */

/* Screen */
extern SDL_Surface *screen;           /**< Stores the main screen SDL structure      */
extern SDL_Surface *virtscreen;       /**< Stores the Chip 8 virtual screen          */
extern int screen_width;              /**< Stores the width of the screen in pixels  */
extern int screen_height;             /**< Stores the height of the screen in pixels */
extern int scale_factor;              /**< The scale factor applied to the screen    */
extern int screen_extended_mode;      /**< Whether the screen is in extended mode    */

/* Colors */
extern Uint32 COLOR_BLACK;            /**< Black pixel color                         */
extern Uint32 COLOR_WHITE;            /**< White pixel color                         */
extern Uint32 COLOR_DGREEN;           /**< Dark green pixel color (for overlay)      */
extern Uint32 COLOR_LGREEN;           /**< Light green pixel color (for overlay)     */
extern SDL_Color COLOR_TEXT;          /**< Text color (white)                        */

/* CPU */
extern chip8regset cpu;               /**< The main emulator CPU                     */
extern SDL_TimerID cpu_timer;         /**< A CPU tick timer                          */
extern unsigned long cpu_interrupt;   /**< The CPU interrupt routine                 */
extern int decrement_timers;          /**< Flags CPU to decrement DELAY and SOUND    */
extern int op_delay;                  /**< Millisecond delay on the CPU              */
extern int awaiting_keypress;         /**< Whether to wait for a keypress event      */

/* Event captures */
extern SDL_Event event;               /**< Stores SDL events                         */

/* Test variables */
extern word tword;
extern word address;

/* P R O T O T Y P E S ********************************************************/

/* cpu.c */
void cpu_reset(void);
int cpu_timerinit(void);
void cpu_execute(void);
void cpu_execute_single(void);

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
int keyboard_isemulatorkey(SDLKey key);
int keyboard_checkforkeypress(int keycode);
void keyboard_processkeydown(SDLKey key);
void keyboard_processkeyup(SDLKey key);

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
void test_keyboard_checkforkeypress_returns_false_on_no_keypress(void);
void test_keyboard_process_keydown(void);
void test_keyboard_process_keyup(void);
void test_keyboard_isemulatorkey(void);

#endif

/* E N D   O F   F I L E ******************************************************/
