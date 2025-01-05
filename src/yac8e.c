/**
 * Copyright (C) 2012-2025 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 * 
 * @file      yac8e.c
 * @brief     Yet Another Chip 8 Emulator
 * @author    Craig Thomas
 */

/* I N C L U D E S ***********************************************************/

#include <getopt.h>
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
    printf("usage: yac8e [-h] [-s] [-j] [-i] [-l] [-c] [-S] ROM\n\n");
    printf("Starts a simple Chip 8 emulator. See README.md for more ");
    printf("information, and\nLICENSE for terms of use.\n\n");
    printf("positional arguments:\n");
    printf("  ROM          the ROM file to load on startup\n\n");
    printf("optional arguments:\n");
    printf("  -h, --help         show this help message and exit\n");
    printf("  -s, --scale N      scales the display by a factor of N\n");
    printf("  -j, --jump_quirks  enables jump quirks\n");
    printf("  -i, --index_quirks enables index quirks\n");
    printf("  -S, --shift_quirks enables shift quirks\n");
    printf("  -l, --logic_quirks enables logic quirks\n");
    printf("  -c, --clip_quirks  enables clip quirks");
}

/******************************************************************************/

/**
 * Parse the command-line options. There are currently 4 recognized options:
 *
 * @param argc the number of arguments on the command line
 * @param argv a pointer to a pointer to the set of command line strings
 * @returns the name of the file to be loaded
 */
char *
parse_options(int argc, char **argv) 
{
    jump_quirks = FALSE;
    shift_quirks = FALSE;
    index_quirks = FALSE;
    clip_quirks = FALSE;
    scale_factor = SCALE_FACTOR;
    op_delay = 0;

    int option_index = 0;
    const char *short_options = ":hjiSslc:";
    static struct option long_options[] =
    {
        {"help",         no_argument,       NULL, 'h'},
        {"jump_quirks",  no_argument,       NULL, 'j'},
        {"index_quirks", no_argument,       NULL, 'i'},
        {"shift_quirks", no_argument,       NULL, 'S'},
        {"scale",        required_argument, NULL, 's'},
        {"logic_quirks", no_argument,       NULL, 'l'},
        {"clip_quirks",  no_argument,       NULL, 'c'},
        {NULL,           0,                 NULL,   0}
    };

    while (TRUE) {
        int option = getopt_long(argc, argv, short_options, long_options, &option_index);

        if (option == -1) {
            break;
        }

        switch (option) {
            case 'h':
                print_help();
                exit(0);
                break;

            case 's':
                scale_factor = atoi(optarg);
                if (scale_factor == 0 || scale_factor < 0 || scale_factor > 20) {
                    printf("Invalid --scale option");
                    print_help();
                    exit(1);
                }
                break;

            case 'j':
                jump_quirks = TRUE;
                break;

            case 'i':
                index_quirks = TRUE;
                break;

            case 'S':
                shift_quirks = TRUE;
                break;

            case 'l':
                logic_quirks = TRUE;
                break;

            case 'c':
                clip_quirks = TRUE;
                break;

            default:
                break;
        }
    }

    int remaining_args = argc - optind;
    if (remaining_args < 1) {
        printf("Expected 1 positional argument (ROM), but found none!\n");
        print_help();
        exit(1);
    }

    char *filename = argv[optind];

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

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Fatal: Unable to initialize SDL\n%s\n", SDL_GetError());
        exit(1);
    }

    if (Mix_OpenAudio(AUDIO_PLAYBACK_RATE, AUDIO_U8, 1, 512) < 0) {
        printf("Fatal: Unable to initialize SDL_mixer\n%s\n", SDL_GetError());
        exit(1);
    }

    if (!memory_init(MEM_SIZE)) {
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
