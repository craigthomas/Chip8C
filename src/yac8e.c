/**
 * Copyright (C) 2012 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 * 
 * @file      yac8e.c
 * @brief     Yet Another Chip 8 Emulator
 * @author    Craig Thomas
 *
 * This project is represents a Chip 8 emulator.
 * In addition to the emulator keys, there are several special keys: 
 *
 *     ESC       Immediately exits the emulator
 *     F1        Debug mode - an overlay will display the current instruction
 *     F2        Trace mode - executes and displays instructions
 *     F12       Run mode - normal execution mode
 *     ->        Steps to the next instruction (while in debug mode)
 */

/* I N C L U D E S ***********************************************************/

#include <stdlib.h>
#include "globals.h"

/* F U N C T I O N S *********************************************************/

/**
 * Loads the specified ROM contained in the file `romfilename` into emulator 
 * memory at the memory offset indicated by `offset`. Returns FALSE on 
 * failure or TRUE on success.
 *
 * @param romfilename the name of the file to load
 * @param offset the memory offset at which to load the file
 * @returns TRUE on success or FALSE on failure
 */
int 
loadrom(char *romfilename, int offset) 
{
    FILE *fp;
    int result = TRUE;

    fp = fopen(romfilename, "r");
    if (fp == NULL) {
        printf("Error: could not open ROM image: %s\n", romfilename);
        result = FALSE; 
    } 
    else {
        while (!feof(fp)) {
            fread(&memory[offset], 1, 1, fp);
            offset++;
        } 
        fclose(fp);
    }
    return result;
}

/*****************************************************************************/

/**
 * Prints out the usage message.
 */
void 
print_help(void) 
{
    printf("usage: yac8e [-h] [-s SCALE] [-d OP_DELAY] [-t] rom\n\n");
    printf("Starts a simple Chip 8 emulator. See README.md for more ");
    printf("information, and\n LICENSE for terms of use.\n\n");
    printf("positional arguments:\n");
    printf("  rom          the ROM file to load on startup\n\n");
    printf("optional arguments:\n");
    printf("  -h           show this help message and exit\n");
    printf("  -s SCALE     the scale factor to apply to the display ");
    printf("(default is 14)\n");
    printf("  -d OP_DELAY  sets the CPU operation to take at least the ");
    printf("specified number of milliseconds to execute (default is 1)\n");
    printf("  -t           starts the CPU up in trace mode\n");
}

/******************************************************************************/

/**
 * Parse the command-line options. There are currently 4 recognized options:
 *
 *   -h      prints out the usage message
 *   -s      applies a scale factor to the window
 *   -d      applies an op delay to the CPU (in milliseconds)
 *   -t      starts the emulator in debug mode
 *
 * @param argc the number of arguments on the command line
 * @param argv a pointer to a pointer to the set of command line strings
 * @returns the name of the file to be loaded
 */
char *
parse_options(int argc, char **argv) 
{
    int arg;
    char *filename = NULL;

    for (arg = 1; arg < argc; arg++) {
        if ((argv[arg][0] == '-') && (strlen(argv[arg]) != 2)) {
            printf("Unrecognized option: %s\n", argv[arg]);
            print_help();
            exit(1);
        }
        else if ((argv[arg][0] == '-') && (strlen (argv[arg]) == 2)) {
            switch (argv[arg][1]) {
                case ('h'):
                    print_help();
                    exit(0);
                    break;
 
                case ('s'):
                    arg++; 
                    if (arg < argc) {
                        scale_factor = atoi(argv[arg]);
                    }
                    break;
 
                case ('d'):
                    arg++;
                    if (arg < argc) {
                        op_delay = atoi(argv[arg]);
                    }
                    break;

                case ('t'):
                    cpu.state = CPU_DEBUG;
                    break;

                default:
                    printf("Unrecognized option: %s\n", argv[arg]);
                    print_help();
                    exit(1);
                    break;
            }
        } 
        else {
            if (filename == NULL) {
                filename = argv[arg];
            } 
            else {
                printf("Unrecognized parameter: %s\n", argv[arg]);
                print_help();
                exit(1);
            }
        }
    }

    if (filename == NULL) {
        printf("ROM file not specified\n");
        print_help();
        exit(1);
    }

    return filename;
}

/* M A I N *******************************************************************/

/**
 * Initalize all the main components of the emulator and start the CPU 
 * execution loop. On shutdown, destory any memory structures created as well 
 * as shut down the SDL library.
 */
int 
main(int argc, char **argv) 
{
    char *filename;

    scale_factor = SCALE_FACTOR;
    cpu_reset();
    cpu.state = CPU_RUNNING;

    filename = parse_options(argc, argv);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Fatal: Unable to initialize SDL\n%s\n", SDL_GetError());
        exit(1);
    }

    if (!memory_init(MEM_4K)) {
        printf ("Fatal: Unable to allocate emulator memory\n");
        SDL_Quit ();
        exit (1);
    }

    if (!loadrom("FONTS.chip8", 0)) {
        printf("Fatal: Could not load FONTS.chip8\n");
        memory_destroy();
        SDL_Quit();
        exit(1);
    }

    if (!loadrom(filename, ROM_DEFAULT)) {
        printf("Fatal: Emulator shutdown due to errors\n");
        memory_destroy();
        SDL_Quit();
        exit(1);
    }

    if (!screen_init()) {
        printf("Fatal: Emulator shutdown due to errors\n");
        memory_destroy();
        SDL_Quit();
        exit(0);
    }

    if (!cpu_timerinit()) {
        printf("Fatal: emulator shutdown due to errors\n");
        memory_destroy();
        SDL_Quit();
        exit(1);
    }

    cpu_execute();

    memory_destroy();
    screen_destroy();
    SDL_Quit();
    return 0;
}

/* E N D   O F   F I L E *****************************************************/
