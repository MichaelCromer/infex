#ifndef INFEX_WORLD_H
#define INFEX_WORLD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_C (64)
#define MAX_R (64)
#define MAX_H (8)
#define MAX_TILES ((MAX_C) * (MAX_R))
#define MAX_VERTS (2 * ((MAX_R + 1) * (MAX_C + 1) - 1))
#define TILE_EDGE_LENGTH (16)
#define ROOT_3 (1.7320508f)
#define DELTA_C (TILE_EDGE_LENGTH * ROOT_3)
#define TILE_ASPECT_RATIO (1.0f)
#define DELTA_R (3.0f * TILE_ASPECT_RATIO * TILE_EDGE_LENGTH / 2.0f)

#define ENEMY_UPDATE_INTERVAL (0.33f)   /* seconds between enemy updates    */
#define ENEMY_GROW_MAX (0.10f)          /* maximum absolute increase        */
#define ENEMY_GROW_FACTOR (0.05f)       /* fraction to increase by          */
#define ENEMY_GROW_THRESHOLD (0.01f)    /* minimum value for growth         */
#define ENEMY_SPLIT_THRESHOLD (0.90f)   /* minimum delta before split       */
#define ENEMY_LEVEL_THRESHOLD (0.10f)   /* minimum delta before level flow  */

void world_initialise(size_t rows, size_t cols);
void world_generate(void);
void world_update(float dt);
size_t grid_size(void);
size_t world_num_rows(void);
size_t world_num_cols(void);
size_t world_index(size_t r, size_t c);
size_t world_row(size_t i);
size_t world_col(size_t i);
Vector2 *world_faces(void);
Color *world_colours(void);
uint8_t *world_heights(void);
uint8_t world_height(size_t i);
Vector2 world_centre(void);
Vector2 world_bounds(void);
float world_scale(void);
float *enemy_state(void);

#endif
