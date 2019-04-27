# Copyright (C) 2012-2019 Craig Thomas
# This project uses an MIT style license - see LICENSE for details.

NAME = yac8e
TESTNAME = test
MAINOBJS = cpu.o keyboard.o memory.o screen.o yac8e.o
TESTOBJS = cpu.o keyboard.o memory.o screen.o cpu_test.o screen_test.o test.o 

override CFLAGS += -Wall -g $(shell sdl-config --cflags)
LDFLAGS += -lSDL_ttf $(shell sdl-config --libs) -lcunit

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
	@- $(RM) $(wildcard *.o)
	@- $(RM) $(wildcard *.gcda)
	@- $(RM) $(wildcard *.gcno)
	@- $(RM) $(wildcard *.gcov)
	@- $(RM) $(NAME)
	@- $(RM) $(TESTNAME)
