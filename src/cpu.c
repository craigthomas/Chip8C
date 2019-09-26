/**
 * Copyright (C) 2012 Craig Thomas
 * This project uses an MIT style license - see the LICENSE file for details.
 *
 * @file      cpu.c
 * @brief     An emulated Chip 8 CPU
 * @author    Craig Thomas
 */

/* I N C L U D E S ************************************************************/

#include <time.h>
#include "globals.h"

/* F U N C T I O N S **********************************************************/

/**
 * This function runs a set of routines based on the SDL timer that is set in
 * `cpu_timerinit`. In this case, the timer will decrement the sound and delay
 * timer registers.
 * 
 * @param interval the number of milliseconds to fire on
 * @param parameters additional parameters to the interrupt routine
 * @returns the new interval at which to fire the timer
 */
Uint32 
cpu_timerinterrupt(Uint32 interval, void *parameters)
{
    decrement_timers = TRUE;
    return interval;
}

/******************************************************************************/

/**
 * Initializes an SDL timer to tick at a rate of 60 times per second. Will call
 * `cpu_timerinterrupt` every time the timer counts down to zero.
 *
 * @returns the newly created SDL timer
 */
int 
cpu_timerinit(void)
{
    int result = TRUE;
    SDL_InitSubSystem(SDL_INIT_TIMER);
    cpu_timer = SDL_AddTimer(17, cpu_timerinterrupt, NULL);

    if (cpu_timer == NULL) {
        printf("Error: could not create timer: %s\n", SDL_GetError());
        result = FALSE;
    }

    return result;
}

/******************************************************************************/

/**
 * Resets the CPU registers.
 */
void 
cpu_reset(void)
{
    /* Reset all general purpose registers */
    cpu.v[0x0] = 0;
    cpu.v[0x1] = 0;
    cpu.v[0x2] = 0;
    cpu.v[0x3] = 0;
    cpu.v[0x4] = 0;
    cpu.v[0x5] = 0;
    cpu.v[0x6] = 0;
    cpu.v[0x7] = 0;
    cpu.v[0x8] = 0;
    cpu.v[0x9] = 0;
    cpu.v[0xA] = 0;
    cpu.v[0xB] = 0;
    cpu.v[0xC] = 0;
    cpu.v[0xD] = 0;
    cpu.v[0xE] = 0;
    cpu.v[0xF] = 0;
 
    /* Reset special registers */
    cpu.i.WORD = 0;
    cpu.sp.WORD = SP_START;
    cpu.dt = 0;
    cpu.st = 0;
    cpu.pc.WORD = CPU_PC_START;
    cpu.oldpc.WORD = CPU_PC_START;
    cpu.operand.WORD = 0;
 
    srand(time(0));
    cpu.state = CPU_PAUSED;

    cpu.opdesc = (char *)malloc(MAXSTRSIZE);
}

/******************************************************************************/

/**
 * Process any events inside the SDL event queue. Will not block waiting for
 * events. Any event not processed by the emulator will be discarded.
 */
void 
cpu_process_sdl_events(void)
{
    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                cpu.state = CPU_STOP;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == QUIT_KEY) {
                    cpu.state = CPU_STOP;
                } 
                else if (event.key.keysym.sym == DEBUG_KEY) {
                    cpu.state = CPU_DEBUG;
                } 
                else if (event.key.keysym.sym == TRACE_KEY) {
                    cpu.state = CPU_TRACE;
                } 
                else if (event.key.keysym.sym == NORMAL_KEY) {
                    cpu.state = CPU_RUNNING;
                } 
                else if (event.key.keysym.sym == STEP_KEY) {
                    cpu.state = CPU_STEP;
                }
                break;

            default:
                break;
        }
    }
}

/******************************************************************************/

/**
 * This function executes a single CPU instruction and returns.
 */

void
cpu_execute_single(void) 
{
    byte x;             /* Stores what is usually the x reg nibble */
    byte y;             /* Stores what is usually the y reg nibble */
    byte src;
    byte tgt;
    byte tbyte;         /* A temporary byte */
    int temp;           /* A general purpose temporary integer */
    int i;              /* Used for FOR loop control */
    int j;              /* Used for FOR loop control */
    int color;          /* Stores whether to turn a pixel on or off */
    int currentcolor;   /* Stores the background pixel value */
    word tword;         /* A temporary word */
    int xcor;           /* The x coordinate to draw a pixel at */
    int ycor;           /* The y coordinate to draw a pixel at */

    cpu.oldpc = cpu.pc;
    
    cpu.operand.BYTE.high = memory_read(cpu.pc.WORD);
    cpu.pc.WORD++;
    cpu.operand.BYTE.low = memory_read(cpu.pc.WORD);
    cpu.pc.WORD++;

    switch (cpu.operand.BYTE.high & 0xF0) {
        /* Misc subroutines */
        case 0x00:
            switch (cpu.operand.BYTE.low) {
                /* 00E0 - CLS */
                /* Clear screen */
                case 0xE0:
                    screen_blank();
                    sprintf(cpu.opdesc, "CLS");
                    break;

                /* 00EE - RTS */
                /* Return from subroutine */
                case 0xEE:
                    cpu.sp.WORD--;
                    cpu.pc.BYTE.high = memory_read(cpu.sp.WORD);
                    cpu.sp.WORD--;
                    cpu.pc.BYTE.low = memory_read(cpu.sp.WORD);
                    sprintf(cpu.opdesc, "RTS");
                    break;

                default:
                    break;
            } 
            break;

        /* 1nnn - JUMP nnn */
        /* Jump to address */
        case 0x10:
            cpu.pc.WORD = (cpu.operand.WORD & 0x0FFF);
            sprintf(cpu.opdesc, "JUMP %03X", cpu.pc.WORD);
            break;

        /* 2nnn - CALL nnn */
        /* Jump to subroutine */
        case 0x20:
            memory_write(cpu.sp, cpu.pc.BYTE.low);
            cpu.sp.WORD++;
            memory_write(cpu.sp, cpu.pc.BYTE.high);
            cpu.sp.WORD++;
            cpu.pc.WORD = (cpu.operand.WORD & 0x0FFF);
            sprintf(cpu.opdesc, "CALL %03X", cpu.pc.WORD);
            break;

        /* 3snn - SKE Vs, nn */
        /* Skip if source register equal value */
        case 0x30:
            src = cpu.operand.BYTE.high & 0xF;
            if (cpu.v[src] == cpu.operand.BYTE.low) {
                cpu.pc.WORD += 2;
            }
            sprintf(cpu.opdesc, "SKE V%X, %02X", src, 
                    cpu.operand.BYTE.low);
            break;

        /* 4snn - SKNE Vs, nn */
        /* Skip if source register contents not equal constant value */
        case 0x40:
            src = cpu.operand.BYTE.high & 0xF;
            if (cpu.v[src] != cpu.operand.BYTE.low) {
                cpu.pc.WORD += 2;
            }
            sprintf(cpu.opdesc, "SKNE V%X, %02X", src, 
                    cpu.operand.BYTE.low);
            break;

        /* 5st0 - SKE Vs, Vt */
        /* Skip if source register value is equal to target register */
        case 0x50:
            src = cpu.operand.BYTE.high & 0xF;
            tgt = (cpu.operand.BYTE.low & 0xF0) >> 4;
            if (cpu.v[src] == cpu.v[tgt]) {
                cpu.pc.WORD += 2;
            }
            sprintf(cpu.opdesc, "SKE V%X, V%X", src, tgt);
            break;

        /* 6snn - LOAD Vs, nn */
        /* Move the constant value into the source register */
        case 0x60:
            src = cpu.operand.BYTE.high & 0xF;
            cpu.v[src] = cpu.operand.BYTE.low;
            sprintf(cpu.opdesc, "LOAD V%X, %02X", src, 
                    cpu.operand.BYTE.low);
            break;

        /* 7snn - ADD Vs, nn */
        /* Add the constant value to the source register */
        case 0x70:
            src = cpu.operand.BYTE.high & 0xF;
            temp = cpu.v[src] + cpu.operand.BYTE.low;
            cpu.v[src] = (temp > 255) ? temp - 256 : temp;
            sprintf(cpu.opdesc, "ADD V%X, %02X", src, 
                    cpu.operand.BYTE.low);
            break;

        /* Logical operations */ 
        case 0x80:
            switch (cpu.operand.BYTE.low & 0x0F) {
                /* 8ts0 - LOAD Vs, Vt */
                /* Move the value of the source register into the target */
                case 0x0: 
                    tgt = cpu.operand.BYTE.high & 0xF;
                    src = (cpu.operand.BYTE.low & 0xF0) >> 4;
                    cpu.v[tgt] = cpu.v[src];
                    sprintf(cpu.opdesc, "LOAD V%X, V%X", tgt, src);
                    break;

                /* 8ts1 - OR Vs, Vt */
                /* Perform a logical OR operation between the source and */
                /* the target register, and store the result in the      */
                /* target register                                       */
                case 0x1:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    src = (cpu.operand.BYTE.low & 0xF0) >> 4;
                    cpu.v[tgt] = cpu.v[tgt] | cpu.v[src];
                    sprintf(cpu.opdesc, "OR V%X, V%X", tgt, src);
                    break;

                /* 8ts2 - AND Vs, Vt */
                /* Perform a logical AND operation between the source and */
                /* the target register, and store the result in the       */
                /* target register                                        */
                case 0x2:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    src = (cpu.operand.BYTE.low & 0xF0) >> 4;
                    cpu.v[tgt] = cpu.v[tgt] & cpu.v[src];
                    sprintf(cpu.opdesc, "AND V%X, V%X", tgt, src);
                    break;

                /* 8ts3 - XOR  Vs, Vt */
                /* Perform a logical XOR operation between the source and */
                /* the target register, and store the result in the       */
                /* target register                                        */
                case 0x3:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    src = (cpu.operand.BYTE.low & 0xF0) >> 4;
                    cpu.v[tgt] = cpu.v[tgt] ^ cpu.v[src];
                    sprintf(cpu.opdesc, "XOR V%X, V%X", tgt, src);
                    break;
                
                /* 8ts4 - ADD  Vt, Vs */ 
                /* Add the value in the source register to the value in   */
                /* the target register, and store the result in the       */
                /* target register. If a carry is generated, set a carry  */
                /* flag in register VF                                    */
                case 0x4:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    src = (cpu.operand.BYTE.low & 0xF0) >> 4;
                    temp = cpu.v[src] + cpu.v[tgt];
                    cpu.v[0xF] = (temp > 255) ? 1 : 0;
                    cpu.v[tgt] = (temp > 255) ? temp - 256 : temp;
                    sprintf(cpu.opdesc, "ADD V%X, V%X", tgt, src);
                    break;

                /* 8ts5 - SUB  Vt, Vs */
                /* Subtract the value in the target register from the     */
                /* value in the source register, and store the result in  */
                /* the target register. If a borrow is NOT generated, set */
                /* a carry flag in register VF                            */
                case 0x5:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    src = (cpu.operand.BYTE.low & 0xF0) >> 4;
                    if (cpu.v[tgt] > cpu.v[src]) {
                        cpu.v[0xF] = 1;
                        cpu.v[tgt] -= cpu.v[src];
                    } 
                    else {
                        cpu.v[0xF] = 0;
                        cpu.v[tgt] = 256 + cpu.v[tgt] - cpu.v[src];
                    }
                    sprintf(cpu.opdesc, "SUB V%X, V%X", tgt, src);
                    break;

                /* 8s06 - SHR  Vs */
                /* Shift the bits in the specified register 1 bit to the  */
                /* right. Bit 0 will be shifted into register vf          */
                case 0x6:
                    src = cpu.operand.BYTE.high & 0xF;
                    cpu.v[0xF] = (cpu.v[src] & 1) ? 1 : 0;
                    cpu.v[src] = cpu.v[src] >> 1;
                    sprintf(cpu.opdesc, "SHR V%X", src);
                    break;

                /* 8ts7 - SUBN Vt, Vs */
                /* Subtract the value in the source register from the     */
                /* value in the target register, and store the result in  */
                /* the target register. If a borrow is NOT generated, set */
                /* a carry flag in register VF                            */
                case 0x7:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    src = (cpu.operand.BYTE.low & 0xF0) >> 4;
                    if (cpu.v[src] < cpu.v[tgt]) {
                        cpu.v[0xF] = 1;
                        cpu.v[tgt] = cpu.v[src] - cpu.v[tgt];
                    } 
                    else {
                        cpu.v[0xF] = 0;
                        cpu.v[tgt] = 256 + cpu.v[src] - cpu.v[tgt];
                    }
                    sprintf(cpu.opdesc, "SUBN V%X, V%X", tgt, src);
                    break;

                /* 8s0E - SHL  Vs */
                /* Shift the bits in the specified register 1 bit to the  */
                /* left. Bit 7 will be shifted into register vf           */
                case 0xE:
                    src = cpu.operand.BYTE.high & 0xF;
                    cpu.v[0xF] = (cpu.v[src] & 0x80) ? 1 : 0;
                    cpu.v[src] = cpu.v[src] << 1;
                    sprintf(cpu.opdesc, "SHL V%X", src);
                    break;

                default:
                    break;
            }
            break;

        /* 9st0 - SKNE Vs, Vt */
        /* Skip if source register is equal to target register */
        case 0x90:
            src = cpu.operand.BYTE.high & 0xF;
            tgt = (cpu.operand.BYTE.low & 0xF0) >> 4;
            if (cpu.v[src] != cpu.v[tgt]) {
                cpu.pc.WORD += 2;
            }
            sprintf(cpu.opdesc, "SKNE V%X, V%X", src, tgt);
            break;

        /* Annn - LOAD I, nnn */
        /* Load index register with constant value */
        case 0xA0:
            cpu.i.WORD = (cpu.operand.WORD & 0x0FFF);
            sprintf(cpu.opdesc, "LOAD I, %03X", (cpu.operand.WORD 
                    & 0x0FFF));
            break;

        /* Bnnn - JUMP [I] + nnn */
        /* Load the program counter with the memory value located at the  */
        /* specified operand plus the value of the index register         */
        case 0xB0:
            cpu.pc.WORD = (cpu.operand.WORD & 0x0FFF) + cpu.i.WORD;
            sprintf(cpu.opdesc, "JUMP I + %03X", (cpu.operand.WORD 
                    & 0x0FFF));
            break;

        /* Ctnn - RAND Vt, nn */
        /* A random number between 0 and 255 is generated. The contents   */
        /* of it are then ANDed with the constant value passed in the     */
        /* operand. The result is stored in the target register           */
        case 0xC0:
            tgt = cpu.operand.BYTE.high & 0xF;
            cpu.v[tgt] = (rand() % 255) & cpu.operand.BYTE.low;
            sprintf(cpu.opdesc, "RAND V%X, %02X", tgt, 
                    (cpu.operand.BYTE.low));
            break;
    
        /* Dxyn - DRAW x, y, n_bytes */ 
        /* Draws the sprite pointed to in the index register at the       */
        /* specified x and y coordinates. Drawing is done via an XOR      */
        /* routine, meaning that if the target pixel is already turned    */
        /* on, and a pixel is set to be turned on at that same location   */
        /* via the draw, then the pixel is turned off. The routine will   */
        /* wrap the pixels if they are drawn off the edge of the screen.  */
        /* Each sprite is 8 bits (1 byte) wide. The n_bytes parameter     */
        /* sets how tall the sprite is. Consecutive bytes in the memory   */
        /* pointed to by the index register make up the bytes of the      */
        /* sprite. Each bit in the sprite byte determines whether a pixel */
        /* is turned on (1) or turned off (0). For example, assume that   */
        /* the index register pointed to the following 7 bytes:           */
        /*                                                                */
        /*                 bit 0 1 2 3 4 5 6 7                            */
        /*                                                                */
        /*     byte 0          0 1 1 1 1 1 0 0                            */
        /*     byte 1          0 1 0 0 0 0 0 0                            */
        /*     byte 2          0 1 0 0 0 0 0 0                            */
        /*     byte 3          0 1 1 1 1 1 0 0                            */
        /*     byte 4          0 1 0 0 0 0 0 0                            */
        /*     byte 5          0 1 0 0 0 0 0 0                            */
        /*     byte 6          0 1 1 1 1 1 0 0                            */
        /*                                                                */
        /* This would draw a character on the screen that looks like an   */
        /* 'E'. The x_source and y_source tell which registers contain    */
        /* the x and y coordinates for the sprite. If writing a pixel to  */
        /* a location causes that pixel to be turned off, then VF will be */
        /* set to 1.                                                      */
        case 0xD0:
            x = cpu.operand.BYTE.high & 0xF;
            y = (cpu.operand.BYTE.low & 0xF0) >> 4;
            tword.WORD = cpu.i.WORD;
            cpu.v[0xF] = 0;

            for (i = 0; i < (cpu.operand.BYTE.low & 0xF); i++) {
                tbyte = memory_read (cpu.i.WORD + i);
                ycor = cpu.v[y] + i;
                ycor = ycor % SCREEN_HEIGHT;

                for (j = 0; j < 8; j++) {
                    xcor = cpu.v[x] + j;
                    xcor = xcor % SCREEN_WIDTH;

                    color = (tbyte & 0x80) ? 1 : 0;
                    currentcolor = screen_getpixel(xcor, ycor);

                    cpu.v[0xF] = (currentcolor && color) ? 1 : cpu.v[0xF];
                    color = color ^ currentcolor;

                    screen_draw(xcor, ycor, color);
            tbyte = tbyte << 1;
                } 
            }
            sprintf(cpu.opdesc, "DRAW V%X, V%X, %X", x, y, 
                    (cpu.operand.BYTE.low & 0xF));
            if ((cpu.state != CPU_DEBUG) && (cpu.state != CPU_TRACE)) {
                screen_refresh(FALSE);
            }
            break;

        /* Keyboard routines */
        case 0xE0:
            switch (cpu.operand.BYTE.low) {
                /* Es9E - SKPR Vs */
                /* Check to see if the key specified in the source        */
                /* register is pressed, and if it is, skips the next      */
                /* instruction                                            */
                case 0x9E:
                    src = cpu.operand.BYTE.high & 0xF;
                    if (keyboard_checkforkeypress (cpu.v[src])) {
                        cpu.pc.WORD += 2;
                    }
                    sprintf(cpu.opdesc, "SKPR V%X", src);
                    break;

                /* EsA1 - SKUP Vs */
                /* Check for the specified keypress in the source         */
                /* register and if it is NOT pressed, will skip the next  */
                /* instruction                                            */
                case 0xA1:
                    src = cpu.operand.BYTE.high & 0xF;
                    if (!keyboard_checkforkeypress (cpu.v[src])) {
                        cpu.pc.WORD += 2;
                    }
                    sprintf(cpu.opdesc, "SKUP V%X", src);
                    break;

                default:
                    break;
            }
            break;

        /* Other I/O Routines */
        case 0xF0:
            switch (cpu.operand.BYTE.low) {
                /* Ft07 - LOAD Vt, DELAY */
                /* Move the value of the delay timer into the target      */
                /* register.                                              */
                case 0x07:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    cpu.v[tgt] = cpu.dt; 
                    sprintf(cpu.opdesc, "LOAD V%X, DELAY", tgt);
                    break;

                /* Ft0A - KEYD Vt */
                /* Stop execution until a key is pressed. Move the value  */
                /* of the key pressed into the specified register         */
                case 0x0A:
                    tgt = cpu.operand.BYTE.high & 0xF;
                    cpu.v[tgt] = keyboard_waitgetkeypress();
                    sprintf(cpu.opdesc, "KEYD V%X", tgt);
                    break;     

                /* Fs15 - LOAD DELAY, Vs */
                /* Move the value stored in the specified source register */
                /* into the delay timer                                   */
                case 0x15:
                    src = cpu.operand.BYTE.high & 0xF;
                    cpu.dt = cpu.v[src];
                    sprintf(cpu.opdesc, "LOAD DELAY, V%X", src);
                    break;             

                /* Fs18 - LOAD SOUND, Vs */
                /* Move the value stored in the specified source register */
                /* into the sound timer                                   */
                case 0x18:
                    src = cpu.operand.BYTE.high & 0xF;
                    cpu.st = cpu.v[src];
                    sprintf(cpu.opdesc, "LOAD SOUND, V%X", src);
                    break;

                /* Fs1E - ADD  I, Vs */
                /* Add the value of the source register into the index    */
                /* register                                               */
                case 0x1E:
                    src = cpu.operand.BYTE.high & 0xF; 
                    cpu.i.WORD += cpu.v[src];
                    sprintf(cpu.opdesc, "ADD I, V%X", src);
                    break;

                /* Fs29 - LOAD I, Vs */
                /* Load the index with the sprite indicated in the source */
                /* register. All sprites are 5 bytes long, so the         */
                /* location of the specified sprite is its index          */
                /* multiplied by 5. Font sprites start at memory index 0  */
                case 0x29:
                    src = cpu.operand.BYTE.high & 0xF;
                    cpu.i.WORD = cpu.v[src] * 5;
                    sprintf(cpu.opdesc, "LOAD I, V%X", src);
                    break;

                /* Fs33 - BCD */
                /* Take the value stored in source and place the digits   */
                /* in the following locations:                            */
                /*                                                        */
                /*      hundreds   -> self.memory[index]                  */
                /*      tens       -> self.memory[index + 1]              */
                /*      ones       -> self.memory[index + 2]              */
                /*                                                        */
                /* For example, if the value is 123, then the following   */
                /* values will be placed at the specified locations:      */
                /*                                                        */
                /*      1 -> self.memory[index]                           */
                /*      2 -> self.memory[index + 1]                       */
                /* 3 -> self.memory[index + 2]                            */
                case 0x33:
                    src = cpu.operand.BYTE.high & 0xF;

                    tword.WORD = cpu.i.WORD;
                    i = cpu.v[src] / 100;
                    memory_write(tword, i);

                    tword.WORD++;
                    i = (cpu.v[src] % 100) / 10;
                    memory_write(tword, i);

                    tword.WORD++;
                    i = (cpu.v[src] % 100) % 10;
                    memory_write(tword, i);
                    sprintf(cpu.opdesc, "BCD V%X (%03d)", src, cpu.v[src]);
                    break;

                /* Fn55 - STOR [I], Vn */
                /* Store all of the n registers in the memory pointed to  */
                /* by the index register. For example, to store all of    */
                /* the V registers, n would be the value 'F'              */
                case 0x55:
                    tword.WORD = cpu.i.WORD;
                    for (i = 0; i <= (cpu.operand.BYTE.high & 0xF); i++) {
                        memory_write(tword, cpu.v[i]);
                        tword.WORD++;
                    }
                    sprintf(cpu.opdesc, "STOR %X", (cpu.operand.BYTE.high 
                            & 0xF));
                    break;

                /* Fn65 - LOAD Vn, [I] */
                /* Read all of the V registers from the memory pointed to */
                /* by the index register. For example, to load all of the */
                /* V registers, n would be 'F'                            */
                case 0x65:
                    temp = cpu.i.WORD;
                    for (i = 0; i <= (cpu.operand.BYTE.high & 0xF); i++) {
                        cpu.v[i] = memory_read(temp);
                        temp++;
                    }
                    sprintf(cpu.opdesc, "LOAD %X", (cpu.operand.BYTE.high 
                            & 0xF));
                    break;
                
                default:
                    break;
            }
            break;

        default:
            break; 
    }
}

/******************************************************************************/

/**
 * This function contains the main CPU execution loop. It is responsible for 
 * checking the SDL event structure for input events. It then fetches and 
 * decodes the next instruction, executes it and restarts the loop. This 
 * process continues until the `cpu.state` flag is set to `CPU_STOP`. It also
 * will decrement timers when the `decrement_timers` flag is set to `TRUE`.
 */
void 
cpu_execute(void)
{
    while (cpu.state != CPU_STOP) {
        cpu_execute_single();

        if ((cpu.state == CPU_DEBUG) || (cpu.state == CPU_TRACE)) {
            screen_refresh(TRUE);
            while (cpu.state == CPU_DEBUG) {
                cpu_process_sdl_events();
                SDL_Delay(20);
            } 
            cpu.state = (cpu.state == CPU_STEP) ? CPU_DEBUG : cpu.state;
        } 
        else {
            SDL_Delay(op_delay);
        }

        if (decrement_timers) {
            cpu.dt -= (cpu.dt > 0) ? 1 : 0;
            cpu.st -= (cpu.st > 0) ? 1 : 0;
            decrement_timers = FALSE;
        }

        cpu_process_sdl_events();
    }
}

/* E N D   O F   F I L E ******************************************************/
