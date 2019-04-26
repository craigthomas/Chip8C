# Yet Another Chip 8 Emulator

[![Build Status](https://travis-ci.org/craigthomas/Chip8C.svg?branch=master&style=flat)](https://travis-ci.org/craigthomas/Chip8C) 
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/befe98f4b3a044a9a0df49aa0fcaf35a)](https://www.codacy.com/app/craig-thomas/Chip8C?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=craigthomas/Chip8C&amp;utm_campaign=Badge_Grade)

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
5. [Customization](#customization)
6. [Troubleshooting](#troubleshooting)
7. [Further Documentation](#further-documentation)
8. [Third Party Copyrights](#third-party-copyrights)

## What is it?

This project is a Chip 8 emulator written in C. The original purpose
of the project was to learn how to write an emulator in a compiled language
so that I could write a Color Computer 3 emulator. Since then, I have written
the emulator in Python 3 (see [Chip8Python](https://github.com/craigthomas/Chip8Python)
as a means of exploring other programming languages. 


## License

Please see the file called LICENSE.


## Compiling

Simply copy the source files to a directory of your choice. In addition to
the source, you will need the following required software packages:

* [GNU C Compiler](http://gcc.gnu.org/) 4.6.3 or later
* [GNU Make](http://www.gnu.org/software/make/) 3.81 or later
* [SDL](http://www.libsdl.org/) 1.2.14 or later
* [SDL TTF Extension](http://www.libsdl.org/projects/SDL_ttf/) 2.0.11 or later

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

### Debug Mode 

You can also ask the emulator to start in debug mode, where each
instruction is disassembled and displayed in the bottom right hand corner
of the screen on a semi-transparent overlay. To do this:

    yac8e /path/to/rom/filename -t

This will start the emulator and pause it after executing the first 
instruction. Pressing the right arrow key will step to the next instruction
and again pause. The emulator will continue this way until you press the
`F2` or `F12` keys (the former will run in trace mode where the overlay will
still be active, while the latter will resume normal execution). Pressing
`F1` will return to debug mode. Debug, trace and normal mode can be 
activated at any time by pressing their respective keys (you do not have
to specify the `-t` switch at startup to enable these modes).


## Customization

The file `keyboard.c` contains the key mapping between the PC keyboard keys
and the Chip 8 emulator keys. Updating the second column of the `keyboard_def`
array will effectively change the key mapping. 


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


## Third Party Copyrights

This project makes use of an unmodified "Vera Mono" font under the 
following license agreement:

    Copyright (c) 2003 by Bitstream, Inc. All Rights Reserved. Bitstream
    Vera is a trademark of Bitstream, Inc.

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of the fonts accompanying this license ("Fonts") and associated
    documentation files (the "Font Software"), to reproduce and distribute
    the Font Software, including without limitation the rights to use,
    copy, merge, publish, distribute, and/or sell copies of the Font
    Software, and to permit persons to whom the Font Software is furnished
    to do so, subject to the following conditions:
    
    The above copyright and trademark notices and this permission notice
    shall be included in all copies of one or more of the Font Software
    typefaces.
    
    The Font Software may be modified, altered, or added to, and in
    particular the designs of glyphs or characters in the Fonts may be
    modified and additional glyphs or characters may be added to the
    Fonts, only if the fonts are renamed to names not containing either
    the words "Bitstream" or the word "Vera".
    
    This License becomes null and void to the extent applicable to Fonts
    or Font Software that has been modified and is distributed under the
    "Bitstream Vera" names.
    
    The Font Software may be sold as part of a larger software package but
    no copy of one or more of the Font Software typefaces may be sold by
    itself.
    
    THE FONT SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
    OF COPYRIGHT, PATENT, TRADEMARK, OR OTHER RIGHT. IN NO EVENT SHALL
    BITSTREAM OR THE GNOME FOUNDATION BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, INCLUDING ANY GENERAL, SPECIAL, INDIRECT, INCIDENTAL,
    OR CONSEQUENTIAL DAMAGES, WHETHER IN AN ACTION OF CONTRACT, TORT OR
    OTHERWISE, ARISING FROM, OUT OF THE USE OR INABILITY TO USE THE FONT
    SOFTWARE OR FROM OTHER DEALINGS IN THE FONT SOFTWARE.
    
    Except as contained in this notice, the names of Gnome, the Gnome
    Foundation, and Bitstream Inc., shall not be used in advertising or
    otherwise to promote the sale, use or other dealings in this Font
    Software without prior written authorization from the Gnome Foundation
    or Bitstream Inc., respectively. For further information, contact:
    fonts at gnome dot org.
