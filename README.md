# Yet Another (Super) Chip 8 Emulator

[![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/craigthomas/Chip8C/c-cpp.yml?style=flat-square&branch=main)](https://github.com/craigthomas/Chip8C/actions)
[![Coverage Status](https://img.shields.io/codecov/c/gh/craigthomas/Chip8C?style=flat-square)](https://codecov.io/gh/craigthomas/Chip8C)
[![Releases](https://img.shields.io/github/release/craigthomas/Chip8C?style=flat-square)](https://github.com/craigthomas/Chip8C/releases)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg?style=flat-square)](https://opensource.org/licenses/MIT)

## Table of Contents

1. [What is it?](#what-is-it)
2. [License](#license)
3. [Compiling](#compiling)
    1. [Big-endian Architectures](#big-endian-architectures)
    2. [Unit Tests](#unit-tests)
4. [Running](#running)
    1. [Screen Scaling](#screen-scaling)
    2. [Execution Delay](#execution-delay)
    3. [Debug Mode](#debug-mode)
5. [Keys](#keys)
    1. [Regular Keys](#regular-keys)
    2. [Debug Keys](#debug-keys)
6. [ROM Compatibility](#rom-compatibility)
7. [Troubleshooting](#troubleshooting)
8. [Further Documentation](#further-documentation)

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

The command-line interface requires a single argument, which is the full
path to a Chip 8 ROM:

    yac8e /path/to/rom/filename

This will start the emulator with the specified ROM. The emulator also
takes optional parameters. 

### Screen Scaling

The `-s` switch will scale the size of the window (the original size at 
1x scale is 64 x 32):

    yac8e /path/to/rom/filename -s 10

The command above will scale the window so that it is 10 times the normal
size. 

### Execution Delay

You may also wish to experiment with the `-d` switch, which instructs
the emulator to add a delay to every operation that is executed. For example,

    yac8e /path/to/rom/filename -d 10

The command above will add a 10 ms delay to every opcode that is executed.
This is useful for very fast computers (note that it is difficult to find
information regarding opcode execution times, as such, I have not attempted
any fancy timing mechanisms to ensure that instructions are executed in a
set amount of time).

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

## ROM Compatibility

Here are the list of public domain ROMs and their current status with the emulator.

| ROM Name          | Works Correctly    | Notes |
| :---------------: | :----------------: | :---: |
| MAZE              | :heavy_check_mark: |       |


## Troubleshooting

* Running `make` fails with errors regarding `SDL` functions.

You will need to ensure that you have SDL 1.2.14 or later installed on your 
computer. Additionally, you will need to make sure that the `sdl-config`
binary is available on your search path. The makefile uses it exclusively to
ensure that the correct `LDFLAGS` and `CFLAGS` are set. If you do not have the
binary, you can try providing the correct flags and paths to the SDL library
on the command line. For example:

    CFLAGS=-I/usr/include/SDL make

The above command would force the make to use `/usr/include/SDL` as a source
of header files during the compile.


* Running `make` fails with errors regarding `TTF` functions.

The project makes use of the `SDL_ttf` extension library. On most systems,
SDL extensions are installed in the same location as the SDL library. However,
if you have installed the TTF extensions in a different location, you will have
to supply the library path and include files in the `LDFLAGS` and `CFLAGS`
variables prior to the build. See above for an example of how to do this.


## Further Documentation

Comments in the source code are written to conform to Doxygen conventions. 
A Doxygen configuration file, along with an associated make target have been
supplied. Simply type:

    make doc

This will create a directory called `doc` with `html` and `latex` directories.
Under `html`, open the `index.html` file in a web browser for more information.
