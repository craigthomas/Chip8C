# Yet Another (Super) Chip 8 Emulator

[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/craigthomas/Chip8C/c-cpp.yml?style=flat-square&branch=main)](https://github.com/craigthomas/Chip8C/actions)
[![Coverage Status](https://img.shields.io/codecov/c/gh/craigthomas/Chip8C?style=flat-square)](https://codecov.io/gh/craigthomas/Chip8C)
[![Releases](https://img.shields.io/github/release/craigthomas/Chip8C?style=flat-square)](https://github.com/craigthomas/Chip8C/releases)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](https://opensource.org/licenses/MIT)

An Octo compatible XO Chip, Super Chip, and Chip 8 emulator.

## Table of Contents

1. [What is it?](#what-is-it)
2. [License](#license)
3. [Compiling](#compiling)
    1. [Big-endian Architectures](#big-endian-architectures)
    2. [Unit Tests](#unit-tests)
4. [Running](#running)
    1. [Running a ROM](#running-a-rom)
    2. [Screen Scaling](#screen-scaling)
    3. [Instructions Per Second](#instructions-per-second)
    4. [Quirks Modes](#quirks-modes)
        1. [Shift Quirks](#shift-quirks)
        2. [Index Quirks](#index-quirks)
        3. [Jump Quirks](#jump-quirks)
        4. [Clip Quirks](#clip-quirks)
        5. [Logic Quirks](#logic-quirks)
5. [Keys](#keys)
    1. [Regular Keys](#regular-keys)
    2. [Debug Keys](#debug-keys)
6. [Troubleshooting](#troubleshooting)
7. [Further Documentation](#further-documentation)

## What is it?

This project is a Super Chip 8 emulator written in C. There are two other versions
of the emulator written in different languages:

* [Chip8Python](https://github.com/craigthomas/Chip8Python)
* [Chip8Java](https://github.com/craigthomas/Chip8Java)

The original goal of these projects was to learn how to code a simple emulator.

In addition to supporting Chip 8 ROMs, the emulator also supports the Super Chip 
8 instruction set. Note that no additional configuration is needed to run a Super 
Chip 8 ROM - simply run the ROM the same way you would run a normal Chip 8 ROM.


## License

This project makes use of an MIT style license. Please see the file called LICENSE
for more information.


## Compiling

Simply copy the source files to a directory of your choice. In addition to
the source, you will need the following required software packages:

* [GNU C Compiler](http://gcc.gnu.org/) 4.6.3 or later
* [GNU Make](http://www.gnu.org/software/make/) 3.81 or later
* [SDL](http://www.libsdl.org/) 2.0.20 or later
* [CUnit](http://cunit.sourceforge.net/doc/index.html) 2.1-3 or later

Note that other C compilers make work as well. To compile the project, open a
command prompt, switch to the source directory, and type:

    make

Assuming that the required packages are installed and available on your search
path, the project should compile without errors. The standard build variables
are available if you wish to customize the build further. For example, to use
an ARM based C compiler, simply override the `CC` variable during the build:

    CC=arm-c-compiler-bin make

Regardless of the compiler, a successful build should result in a single binary
in the source directory called:

    yac8e

The binary stands for "Yet Another Chip 8 Emulator".

### Big-endian Architectures

If you plan to run the Chip 8 emulator on a big-endian architecture, you will 
need to compile with the following flag:

    make CFLAGS=-DWORDS_BIGENDIAN

### Unit Tests

To run the unit test suite, use the make target of `test`:

    make test


## Running

### Running a ROM

The command-line interface requires a single argument, which is the full
path to a Chip 8 ROM:

    yac8e /path/to/rom/filename

This will start the emulator with the specified ROM. The emulator also
takes optional parameters. 

### Screen Scaling

The `-s` or `--scale` switch will scale the size of the window (the original size at 
1x scale is 64 x 32):

    yac8e /path/to/rom/filename -s 10

The command above will scale the window so that it is 10 times the normal
size. 

### Instructions Per Second

The `-t` or `--ticks` switch will limit the number of instructions per second
that the emulator is allowed to run. By default, the value is set to 1,000. 
Minimum values are 100. Use this switch to adjust the running time of ROMs
that execute too quickly. For simplicity, each instruction is assumed to 
take the same amount of time.  

### Quirks Modes

Over time, various extensions to the Chip8 mnemonics were developed, which
resulted in an interesting fragmentation of the Chip8 language specification.
As discussed in Octo's [Mastering SuperChip](https://github.com/JohnEarnest/Octo/blob/gh-pages/docs/SuperChip.md)
documentation, one version of the SuperChip instruction set subtly changed
the meaning of a few instructions from their original Chip8 definitions.
This change went mostly unnoticed for many implementations of the Chip8
language. Problems arose when people started writing programs using the
updated language model - programs written for "pure" Chip8 ceased to
function correctly on emulators making use of the altered specification.

To address this issue, [Octo](https://github.com/JohnEarnest/Octo) implements
a number of _quirks_ modes so that all Chip8 software can run correctly,
regardless of which specification was used when developing the Chip8 program.
This same approach is used here, such that there are several `quirks` flags
that can be passed to the emulator at startup to force it to run with
adjustments to the language specification.

Additional quirks and their impacts on the running Chip8 interpreter are
examined in great depth at Chromatophore's [HP48-Superchip](https://github.com/Chromatophore/HP48-Superchip)
repository. Many thanks for this detailed explanation of various quirks
found in the wild!

#### Shift Quirks

The `--shift_quirks` flag will change the way that register shift operations work.
In the original language specification two registers were required: the
destination register `x`, and the source register `y`. The source register `y`
value was shifted one bit left or right, and stored in `x`. For example,
shift left was defined as:

    Vx = Vy << 1

However, with the updated language specification, the source and destination
register are assumed to always be the same, thus the `y` register is ignored and
instead the value is sourced from `x` as such:

    Vx = Vx << 1

#### Index Quirks

The `--index_quirks` flag controls whether post-increments are made to the index register
following various register based operations. For  load (`Fn65`) and store (`Fn55`) register
operations, the original specification for the Chip8 language results in the index
register being post-incremented by the number of registers stored. With the Super
Chip8 specification, this behavior is not always adhered to. Setting `--index_quirks`
will prevent the post-increment of the index register from occurring after either of these
instructions.


#### Jump Quirks

The `--jump_quirks` controls how jumps to various addresses are made with the jump (`Bnnn`)
instruction. In the original Chip8 language specification, the jump is made by taking the
contents of register 0, and adding it to the encoded numeric value, such as:

    PC = V0 + nnn

With the Super Chip8 specification, the highest 4 bits of the instruction encode the
register to use (`Bxnn`) such. The behavior of `--jump_quirks` becomes:

    PC = Vx + nn


#### Clip Quirks

The `--clip_quirks` controls whether sprites are allowed to wrap around the display.
By default, sprits will wrap around the borders of the screen. If turned on, then
sprites will not be allowed to wrap.


#### Logic Quirks

The `--logic_quirks` controls whether the F register is cleared after logic operations
such as AND, OR, and XOR. By default, F is left undefined following these operations.
With the flag turned on, F will always be cleared.

## Keys

The file `keyboard.c` contains the key mapping between the PC keyboard keys
and the Chip 8 emulator keys. Updating the second column of the `keyboard_def`
array will effectively change the key mapping. 

There are two sets of keys that the emulator uses: debug keys and regular
keys.

### Regular Keys

The original Chip 8 had a keypad with the numbered keys 0 - 9 and A - F (16
keys in total). The original key configuration was as follows:


| `1` | `2` | `3` | `C` |
|-----|-----|-----|-----|
| `4` | `5` | `6` | `D` |
| `7` | `8` | `9` | `E` |
| `A` | `0` | `B` | `F` |

The Chip8 emulator maps them to the following keyboard keys by default:

| `1` | `2` | `3` | `4` |
|-----|-----|-----|-----|
| `Q` | `W` | `E` | `R` |
| `A` | `S` | `D` | `F` |
| `Z` | `X` | `C` | `V` |

### Debug Keys

In addition to the key mappings specified in the configuration file, there are additional
keys that impact the execution of the emulator.

| Keyboard Key | Effect             |
| :----------: |--------------------|
| `ESC`        | Quits the emulator |

## Troubleshooting

* Running `make` fails with errors regarding `SDL` functions.

You will need to ensure that you have SDL 2.20 or later installed on your 
computer. Additionally, you will need to make sure that the `sdl2-config`
binary is available on your search path. The makefile uses it exclusively to
ensure that the correct `LDFLAGS` and `CFLAGS` are set. If you do not have the
binary, you can try providing the correct flags and paths to the SDL library
on the command line. For example:

    CFLAGS=-I/usr/include/SDL2 make

The above command would force the make to use `/usr/include/SDL2` as a source
of header files during the compile. Additionally, library flags need to be 
set as well, such as:

    LDFLAGS=-lSDL2 -lSDL2_mixer make


## Further Documentation

Comments in the source code are written to conform to Doxygen conventions. 
A Doxygen configuration file, along with an associated make target have been
supplied. Simply type:

    make doc

This will create a directory called `doc` with `html` and `latex` directories.
Under `html`, open the `index.html` file in a web browser for more information.
