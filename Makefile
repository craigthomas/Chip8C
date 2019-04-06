# Copyright (C) 2012 Craig Thomas
# This project uses an MIT style license - see LICENSE for details.

NAME = yac8e
SRCS = $(wildcard *.c)
OBJS = ${SRCS:.c=.o}

override CFLAGS += -Wall -g $(shell sdl-config --cflags)
LDFLAGS += -lSDL_ttf $(shell sdl-config --libs)

.PHONY: all doc clean

all: $(NAME)

yac8e: $(OBJS)
	$(LINK.c) -o $(NAME) $(OBJS) $(LDFLAGS)

doc:
	doxygen doxygen.conf

clean:
	@- $(RM) $(OBJS)
	@- $(RM) $(NAME)
