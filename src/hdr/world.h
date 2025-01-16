#ifndef INFEX_WORLD_H
#define INFEX_WORLD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>


/* GRID */
size_t grid_size(void);
float grid_scale(void);
Vector2 *grid_faces(void);

/* MAP */
Color *map_colours(void);

/* ENEMY */
float *enemy_state(void);
float enemy_score(void);

/* WORLD */
void world_initialise(size_t rows, size_t cols);
void world_generate(void);
void world_update(float dt);
Vector2 world_centre(void);
Vector2 world_bounds(void);

#endif
