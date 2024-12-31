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
   if (memory != NULL) {
      free(memory);
      memory = NULL;
   }
   memory = (byte *)malloc(sizeof (byte) * memorysize);
   return memory != NULL;
}

/*****************************************************************************/

/**
 * Frees the emulator memory.
 */
void 
memory_destroy(void) 
{
   free(memory);
   memory = NULL;
}

/* E N D   O F   F I L E *****************************************************/
