/**
 * Copyright (C) 2012 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      memory.c
 * @brief     Routines for addressing emulator memory
 * @author    Craig Thomas
 *
 * This file contains several routines for initializing and manipulating
 * emulator memory. Memory must first be initialized using the `memory_init`
 * function. Once intialized, all memory read and write functions should be
 * performed through the memory routines `memory_read` and `memory_write`. 
 * The reason for this requirement is due to the fact that some architectures
 * use memory mapped I/O routines. While the Chip 8 does not have this 
 * restriction, it is still good practice (for example, the Color Computer 3
 * has memory mapped I/O, which is another open project). When the memory for
 * the emulator is no longer needed, it may be freed using `memory_destroy`.
 */

/* I N C L U D E S ***********************************************************/

#include <malloc.h>
#include "globals.h"

/* F U N C T I O N S *********************************************************/

/**
 * Attempts to malloc a memory block to use as emulator memory. Returns TRUE on
 * success or FALSE on failure.
 *
 * @param memorysize the size of memory to allocate for the emulator
 */
int 
memory_init(int memorysize) 
{
   memory = (byte *)malloc(sizeof (byte) * memorysize);
   if (!memory) {
      return FALSE;
   }

   return TRUE;
}

/*****************************************************************************/

/**
 * Attempts to read one byte of memory at the requested address. Returns the 
 * byte read from memory.
 *
 * @param address the address in memory to read from
 * @return the value read from the specified memory location
 */
inline byte 
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
inline void 
memory_write(register word address, register byte value) 
{
   memory[address.WORD] = value;
}

/*****************************************************************************/

/**
 * Frees the emulator memory.
 */
void 
memory_destroy(void) 
{
   free(memory);
}

/* E N D   O F   F I L E *****************************************************/
