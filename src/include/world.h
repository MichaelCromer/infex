#ifndef INFEX_WORLD_H
#define INFEX_WORLD_H

#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void world_initialise(size_t rows, size_t cols);
bool world_is_null(void);
size_t world_num_tiles(void);
Vector2 *world_centres(void);
uint8_t *world_heights(void);

#endif
