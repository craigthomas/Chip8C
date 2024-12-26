/**
 * Copyright (C) 2012-2024 Craig Thomas
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

    /* Reset all RPL storage values */
    cpu.rpl[0x0] = 0;
    cpu.rpl[0x1] = 0;
    cpu.rpl[0x2] = 0;
    cpu.rpl[0x3] = 0;
    cpu.rpl[0x4] = 0;
    cpu.rpl[0x5] = 0;
    cpu.rpl[0x6] = 0;
    cpu.rpl[0x7] = 0;
    cpu.rpl[0x8] = 0;
    cpu.rpl[0x9] = 0;
    cpu.rpl[0xA] = 0;
    cpu.rpl[0xB] = 0;
    cpu.rpl[0xC] = 0;
    cpu.rpl[0xD] = 0;
    cpu.rpl[0xE] = 0;
    cpu.rpl[0xF] = 0;
 
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

    if (cpu.opdesc != NULL) {
        free(cpu.opdesc);
        cpu.opdesc = NULL;
    }
    cpu.opdesc = (char *)malloc(MAXSTRSIZE);
    awaiting_keypress = FALSE;
}

/******************************************************************************/

/**
 * Process any events inside the SDL event queue. Will not block waiting for
 * events. Any event not processed by the emulator will be discarded.
 */
void 
cpu_process_sdl_events(void)
{
    int emulatorkey;
    SDLKey key;

    if (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                cpu.state = CPU_STOP;
                break;

            case SDL_KEYDOWN:
                key = event.key.keysym.sym;
                if (key == QUIT_KEY) {
                    cpu.state = CPU_STOP;
                } 
                keyboard_processkeydown(key);
                if (awaiting_keypress) {
                    emulatorkey = keyboard_isemulatorkey(key);
                    if (emulatorkey != -1) {
                        int x = cpu.operand.BYTE.high & 0xF;
                        cpu.v[x] = emulatorkey;
                        awaiting_keypress = FALSE;
                    }
                }
                break;

            case SDL_KEYUP:
                keyboard_processkeyup(event.key.keysym.sym);
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
    byte src;           /* Source register */
    byte tgt;           /* Target register */
    byte tbyte;         /* A temporary byte */
    int temp;           /* A general purpose temporary integer */
    int i;              /* Used for FOR loop control */
    int j;              /* Used for FOR loop control */
    int k;              /* Used for FOR loop control */
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
                /* 00Cn - SCRD */
                /* Scroll Down */
                case 0xC0:
                case 0xC1:
                case 0xC2:
                case 0xC3:
                case 0xC4:
                case 0xC5:
                case 0xC6:
                case 0xC7:
                case 0xC8:
                case 0xC9:
                case 0xCA:
                case 0xCB:
                case 0xCC:
                case 0xCD:
                case 0xCE:
                case 0xCF:
                    temp = cpu.operand.BYTE.low & 0x0F;
                    screen_scroll_down(temp);
                    sprintf(cpu.opdesc, "SCRD %d", temp);
                    break;

                /* 00E0 - CLS */
                /* Clear screen */
                case 0xE0:
                    screen_blank();
                    sprintf(cpu.opdesc, "CLS");
                    break;

                case 0xEE:
                    return_from_subroutine();
                    break;

                /* 00FB - SCRR */
                /* Scroll right */
                case 0xFB:
                    screen_scroll_right();
                    sprintf(cpu.opdesc, "SCRR");
                    break;

                /* 00FC - SCRL */
                /* Scroll left */
                case 0xFC:
                    screen_scroll_left();
                    sprintf(cpu.opdesc, "SCRL");
                    break;

                /* 00FD - EXIT */
                /* Exit interpreter */
                case 0xFD:
                    cpu.state = CPU_STOP;
                    sprintf(cpu.opdesc, "EXIT");
                    break;

                /* 00FE - EXTD */
                /* Disable extended mode */
                case 0xFE:
                    screen_set_normal_mode();
                    sprintf(cpu.opdesc, "EXTD");
                    break;

                /* 00FF - EXTE */
                /* Enable extended mode */
                case 0xFF:
                    screen_set_extended_mode();
                    sprintf(cpu.opdesc, "EXTE");
                    break;

                default:
                    break;
            } 
            break;

        case 0x10:
            jump_to_address();
            break;

        case 0x20:
            jump_to_subroutine();
            break;

        case 0x30:
            skip_if_register_equal_value();
            break;

        case 0x40:
            skip_if_register_not_equal_value();
            break;

        case 0x50:
            skip_if_register_equal_register();
            break;

        case 0x60:
            move_value_to_register();
            break;

        case 0x70:
            add_value_to_register();
            break;

        /* Logical operations */ 
        case 0x80:
            switch (cpu.operand.BYTE.low & 0x0F) {
                case 0x0: 
                    move_register_into_register();
                    break;

                case 0x1:
                    logical_or();
                    break;

                case 0x2:
                    logical_and();
                    break;

                case 0x3:
                    exclusive_or();
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
            tbyte = cpu.operand.BYTE.low & 0xF;
            cpu.v[0xF] = 0;

            if (screen_is_extended_mode() && tbyte == 0) {
                for (i = 0; i < 16; i++) {
                    for (k = 0; k < 2; k++) {
                        tbyte = memory_read(cpu.i.WORD + (i * 2) + k);
                        ycor = cpu.v[y] + i;
                        ycor = ycor % screen_get_height();

                        for (j = 0; j < 8; j++) {
                            xcor = cpu.v[x] + j + (k * 8);
                            xcor = xcor % screen_get_width();

                            color = (tbyte & 0x80) ? 1 : 0;
                            currentcolor = screen_get_pixel(xcor, ycor);

                            cpu.v[0xF] = (currentcolor && color) ? 1 : cpu.v[0xF];
                            color = color ^ currentcolor;

                            screen_draw(xcor, ycor, color);
                            tbyte = tbyte << 1;
                        } 
                    }
                }
                sprintf(cpu.opdesc, "DRAWEX V%X, V%X, %X", x, y, 
                        (cpu.operand.BYTE.low & 0xF));
            } else {
                for (i = 0; i < (cpu.operand.BYTE.low & 0xF); i++) {
                    tbyte = memory_read (cpu.i.WORD + i);
                    ycor = cpu.v[y] + i;
                    ycor = ycor % screen_get_height();

                    for (j = 0; j < 8; j++) {
                        xcor = cpu.v[x] + j;
                        xcor = xcor % screen_get_width();

                        color = (tbyte & 0x80) ? 1 : 0;
                        currentcolor = screen_get_pixel(xcor, ycor);

                        cpu.v[0xF] = (currentcolor && color) ? 1 : cpu.v[0xF];
                        color = color ^ currentcolor;

                        screen_draw(xcor, ycor, color);
                        tbyte = tbyte << 1;
                    } 
                }
                sprintf(cpu.opdesc, "DRAW V%X, V%X, %X", x, y, 
                        (cpu.operand.BYTE.low & 0xF));
            }

            screen_refresh(FALSE);
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
                    awaiting_keypress = TRUE;
                    tgt = cpu.operand.BYTE.high & 0xF;
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
                    tbyte = cpu.operand.BYTE.high & 0xF;
                    for (i = 0; i <= tbyte; i++) {
                        cpu.v[i] = memory_read(temp);
                        temp++;
                    }
                    sprintf(cpu.opdesc, "LOAD %X", tbyte);
                    break;

                /* Fn75 - SRPL n */
                /* Stores the values from n number of registers           */
                /* (starting from 0) to the RPL store. */
                case 0x75:
                    tbyte = cpu.operand.BYTE.high & 0xF;
                    for (i = 0; i <= tbyte; i++) {
                        cpu.rpl[i] = cpu.v[i];
                    }
                    sprintf(cpu.opdesc, "SRPL %X", tbyte);
                    break;

                /* Fn85 - LRPL n */
                /* Loads n number of registers from the RPL store to      */
                /* their respective registers. */
                case 0x85:
                    tbyte = cpu.operand.BYTE.high & 0xF;
                    for (i = 0; i <= tbyte; i++) {
                        cpu.v[i] = cpu.rpl[i];
                    }
                    sprintf(cpu.opdesc, "LRPL %X", tbyte);
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
 * 00EE - RTS
 * 
 * Return from subroutine. Pop the current value in the stack off of the 
 * stack, and set the program counter to the value popped.
 */
void
return_from_subroutine(void)
{
    cpu.sp.WORD--;
    cpu.pc.BYTE.high = memory_read(cpu.sp.WORD);
    cpu.sp.WORD--;
    cpu.pc.BYTE.low = memory_read(cpu.sp.WORD);
    sprintf(cpu.opdesc, "RTS");
}

/******************************************************************************/

/**
 * 1nnn - JUMP nnn 
 * 
 * Jump to address.
 */
void
jump_to_address(void)
{
    cpu.pc.WORD = (cpu.operand.WORD & 0x0FFF);
    sprintf(cpu.opdesc, "JUMP %03X", cpu.pc.WORD);
}

/******************************************************************************/

/**
 * 2nnn - CALL nnn
 * 
 * Jump to subroutine. Save the current program counter on the stack.
 */
void
jump_to_subroutine(void)
{
    memory_write(cpu.sp, cpu.pc.WORD & 0x00FF);
    cpu.sp.WORD++;
    memory_write(cpu.sp, (cpu.pc.WORD & 0xFF00) >> 8);
    cpu.sp.WORD++;
    cpu.pc.WORD = (cpu.operand.WORD & 0x0FFF);
    sprintf(cpu.opdesc, "CALL %03X", cpu.pc.WORD);
}

/******************************************************************************/

/**
 * 3xnn - SKE Vx, nn
 * 
 * Skip if source register equal constant value. The program counter
 * is updated to skip the next instruction by advancing it by 2 bytes.
 */ 
void
skip_if_register_equal_value(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    if (cpu.v[x] == (cpu.operand.WORD & 0x00FF)) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKE V%X, %02X", x, cpu.operand.BYTE.low);
}

/******************************************************************************/

/**
 * 4xnn - SKNE Vx, nn
 * 
 * Skip if source register contents not equal constant value. The program
 * counter is updated to skip the next instruction by advancing it by 2 bytes.
 */
void
skip_if_register_not_equal_value(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    if (cpu.v[x] != (cpu.operand.WORD & 0x00FF)) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKNE V%X, %02X", x, cpu.operand.BYTE.low);
}

/******************************************************************************/

/**
 * 5xy0 - SKE Vx, Vy
 * 
 * Skip if source register value is equal to target register. The program 
 * counter is updated to skip the next instruction by advancing it by 2 bytes.
 */
void 
skip_if_register_equal_register(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    if (cpu.v[x] == cpu.v[y]) {
        cpu.pc.WORD += 2;
    }
    sprintf(cpu.opdesc, "SKE V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 6xnn - LOAD Vx, nn
 * 
 * Move the constant value into the specified register.
 */
void
move_value_to_register(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.v[x] = cpu.operand.WORD & 0x00FF;
    sprintf(cpu.opdesc, "LOAD V%X, %02X", x, (cpu.operand.WORD & 0x00FF));
}
            
/******************************************************************************/

/**
 * 7xnn - ADD Vx, nn
 * 
 * Add the constant value to the source register.
 */
void
add_value_to_register(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    cpu.v[x] = cpu.v[x] + ((cpu.operand.WORD & 0x00FF) % 256);
    sprintf(cpu.opdesc, "ADD V%X, %02X", x, cpu.operand.BYTE.low);
}

/******************************************************************************/

/**
 * 8xy0 - LOAD Vx, Vy
 *
 * Move the y register into the x register. 
 */
void
move_register_into_register(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] = cpu.v[y];
    sprintf(cpu.opdesc, "LOAD V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xy1 - OR Vx, Vy
 * 
 * Perform a logical OR operation between x and y and store the result
 * in x.
 */
void
logical_or(void) 
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] |= cpu.v[y];
    sprintf(cpu.opdesc, "OR V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xy2 - AND Vx, Vy
 * 
 * Perform a logical AND between x and y and store the result in x.
 */
void
logical_and(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] &= cpu.v[y];
    sprintf(cpu.opdesc, "AND V%X, V%X", x, y);
}

/******************************************************************************/

/**
 * 8xy3 - XOR  Vx, Vy
 * 
 * Perform a logical XOR between x and y and store the result in x.
 */
void
exclusive_or(void)
{
    int x = (cpu.operand.WORD & 0x0F00) >> 8;
    int y = (cpu.operand.WORD & 0x00F0) >> 4;
    cpu.v[x] ^= cpu.v[y];
    sprintf(cpu.opdesc, "XOR V%X, V%X", x, y);
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
        if (awaiting_keypress != 1) {
            cpu_execute_single();
            if (decrement_timers) {
                cpu.dt -= (cpu.dt > 0) ? 1 : 0;
                cpu.st -= (cpu.st > 0) ? 1 : 0;
                decrement_timers = FALSE;
            }
        }
        cpu_process_sdl_events();
    }
}

/* E N D   O F   F I L E ******************************************************/
