/**
 * @file      globals.h
 * @brief     Global parameters and variables for the emulator
 * @author    Craig Thomas
 * @copyright MIT style license - see the LICENSE file for details
 * @copyright @verbinclude LICENSE
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
#define ROM_DEFAULT    0x200	 /**< Defines the default ROM load point      */

/* Screen */
#define SCREEN_HEIGHT      32    /**< Default screen height                   */
#define SCREEN_WIDTH       64    /**< Default screen width                    */
#define SCREEN_DEPTH       32    /**< Colour depth in BPP                     */
#define SCALE_FACTOR       14    /**< Scaling for the window size             */
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
#define DEBUG_KEY  SDLK_F1     /**< Puts emulator into debug mode             */
#define TRACE_KEY  SDLK_F2     /**< Puts emulator into trace mode             */
#define NORMAL_KEY SDLK_F12    /**< Returns emulator to normal running mode   */
#define STEP_KEY   SDLK_RIGHT  /**< Runs next instruction (in debug mode)     */

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

/* keyboard.c */
int keyboard_waitgetkeypress(void);
int keyboard_checkforkeypress(int keycode);

/* memory.c */
int memory_init(int memorysize);
void memory_destroy(void);
inline void memory_write(register word address, register byte value);
inline byte memory_read(register int address);

/* screen.c */
int screen_init(void);
void screen_clear(SDL_Surface *surface, Uint32 color); 
void screen_blank(void);
int screen_getpixel(int x, int y);
void screen_draw(int x, int y, int color);
void screen_refresh(int overlay_on);

#endif

/* E N D   O F   F I L E ******************************************************/
