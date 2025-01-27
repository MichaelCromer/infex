CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
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

dev: CFLAGS += -g
dev: $(BLDDIR)/$(TARGET)

prod: CFLAGS += -O3
prod: $(BLDDIR)/$(TARGET)

clean:
	rm -rf $(BLDDIR)
.PHONY: clean

tags:
	rm -rf tags
	ctags $(SRC) $(HDR) $(LIBDIR)/clay/clay.h $(LIBDIR)/clay/renderers/raylib/*.c
.PHONY: tags
