#ifndef INFEX_WORLD_H
#define INFEX_WORLD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "infex.h"

void world_initialise(size_t rows, size_t cols);
void world_generate(void);
size_t world_num_faces(void);
size_t world_num_vertices(void);
size_t world_num_rows(void);
size_t world_num_cols(void);
size_t world_index(size_t r, size_t c);
size_t world_row(size_t i);
size_t world_col(size_t i);
Vector2 *world_faces(void);
Vector2 *world_vertices(void);
Color *world_colours(void);
uint8_t *world_heights(void);
Vector2 world_centre(void);
Vector2 world_bounds(void);
float world_scale(void);

#endif
