#ifndef INFEX_WORLD_H
#define INFEX_WORLD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "infex.h"

void world_initialise(size_t rows, size_t cols);
size_t world_num_tiles(void);
Vector2 *world_faces(void);
uint8_t *world_heights(void);
Vector2 world_centre(void);
Vector2 world_bounds(void);

#endif
