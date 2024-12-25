CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g
CLIBS = -lraylib -lm

SRCDIR = src
BLDDIR = build
TARGET = infex

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:%.c=$(BLDDIR)/%.o)

$(BLDDIR)/$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(CLIBS)

$(BLDDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BLDDIR)
	make

format:
	indent -npro -linux -l88 -nut -i4 -cli4 $(shell find src -name "*.[ch]")
	find -name '*~' -delete
	sed -i '/ + \| - /s/ \([*\/]\) /\1/g' $(shell find src -name "*.[ch]")
