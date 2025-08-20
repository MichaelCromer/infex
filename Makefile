CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
CLIBS = -lraylib -lm

DIR_SRC = src
DIR_HDR = $(DIR_SRC)/hdr
DIR_LIB = $(DIR_SRC)/lib
DIR_BLD = bld
DIR_OBJ = $(DIR_BLD)/obj
TARGET = $(DIR_BLD)/infex

SRC = $(wildcard $(DIR_SRC)/*.c)
HDR = $(wildcard $(DIR_SRC)/hdr/*.h)
OBJ = $(SRC:$(DIR_SRC)/%.c=$(DIR_OBJ)/%.o)


$(TARGET): $(OBJ) | $(DIR_BLD)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(CLIBS)


$(DIR_OBJ)/%.o: $(DIR_SRC)/%.c | $(DIR_OBJ)
	$(CC) $(CFLAGS) -c $< -o $@


$(DIR_BLD) $(DIR_OBJ): ; mkdir -p $@


.PHONY: dev
dev : CFLAGS += -g -fsanitize=address,leak,undefined
dev : clean $(TARGET)


prod: CFLAGS += -O3
prod: clean $(TARGET)


.PHONY: clean
clean:
	rm -rf $(DIR_BLD)


.PHONY: tags
tags:
	ctags $(SRC) $(HDR) $(DIR_LIB)/clay/clay.h $(DIR_LIB)/clay/renderers/raylib/*.c
