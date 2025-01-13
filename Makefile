CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g
CLIBS = -lraylib -lm

SRCDIR = src
BLDDIR = build
LIBDIR = src/lib
TARGET = infex

SRC = $(wildcard $(SRCDIR)/*.c)
HDR = $(wildcard $(SRCDIR)/hdr/*.h)
OBJ = $(SRC:%.c=$(BLDDIR)/%.o)

$(BLDDIR)/$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(CLIBS)

$(BLDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean format tags
clean:
	rm -rf $(BLDDIR)
	make

format:
	indent -npro -linux -l88 -nut -i4 -cli4 $(shell find src -name "*.[ch]")
	find -name '*~' -delete
	sed -i '/ + \| - /s/ \([*\/]\) /\1/g' $(shell find src -name "*.[ch]")

tags:
	rm -rf tags
	ctags $(SRC) $(HDR) $(LIBDIR)/clay/clay.h
