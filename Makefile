# Copyright (C) 2012-2019 Craig Thomas
# This project uses an MIT style license - see LICENSE for details.

NAME = yac8e
TESTNAME = test
MAINOBJS = src/cpu.o src/keyboard.o src/memory.o src/screen.o src/yac8e.o src/globals.o
TESTOBJS = src/cpu.o src/keyboard.o src/memory.o src/screen.o src/cpu_test.o src/screen_test.o src/test.o src/keyboard_test.o src/globals.o

override CFLAGS += -Wall -g $(shell sdl-config --cflags)
LDFLAGS += $(shell sdl-config --libs) -lcunit -lm

.PHONY: all doc clean

all: $(NAME)

yac8e: $(MAINOBJS)
	$(LINK.c) -o $(NAME) $(MAINOBJS) $(LDFLAGS)

test: $(TESTOBJS)
	$(LINK.c) -o $(TESTNAME) $(TESTOBJS) $(LDFLAGS)
	./$(TESTNAME)

doc:
	doxygen doxygen.conf

clean:
	@- $(RM) $(wildcard src/*.o)
	@- $(RM) $(wildcard src/*.gcda)
	@- $(RM) $(wildcard src/*.gcno)
	@- $(RM) $(wildcard *.gcov)
	@- $(RM) $(NAME)
	@- $(RM) $(TESTNAME)
