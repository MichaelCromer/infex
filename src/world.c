#include <math.h>
#include <string.h>

#include "include/world.h"

#define MAX_U (64)
#define MAX_V (64)
#define MAX_H (8)
#define MAX_TILES ((MAX_U) * (MAX_V))
#define UV_SCALE (32)
#define ROOT_3 (1.7320508f)
#define DELTA_U (UV_SCALE * ROOT_3)
#define DELTA_V (3.0f * UV_SCALE / 2.0f)

/* scalar function map data */
Vector2 faces[MAX_TILES] = { 0 };       /* geometric locations on-screen of tile faces */
Color colours[MAX_TILES] = { 0 };       /* geometric locations on-screen of tile faces */
uint8_t heights[MAX_TILES] = { 0 };     /* heightmap of tiles */

/* current state */
size_t num_tiles = 0;
size_t num_rows = 0, num_cols = 0;

Vector2 centre = { 0 };
Vector2 bounds = { 0 };         /* lower bounds always 0, upper bounds given here */

void world_initialise(size_t rows, size_t cols)
{
    num_rows = rows, num_cols = cols;
    num_tiles = rows * cols;

    float u = 0, v = 0;
    int i = 0;
    for (size_t r = 0; r < rows; r++) {
        u = (r % 2) ? (DELTA_U / 2) : 0.0f;
        for (size_t c = 0; c < cols; c++) {
            faces[i] = (Vector2) {
            u, v};
            heights[i] = 0;
            u += DELTA_U;
            i++;
        }
        v += DELTA_V;
    }

    bounds = (Vector2) {
    cols *DELTA_U, rows * DELTA_V};
    centre = (Vector2) {
    bounds.x / 2.0f, bounds.y / 2.0f};
}

int world_index(int r, int c)
{
    return (num_cols*r) + c;
}

size_t world_num_tiles(void)
{
    return num_tiles;
}

Vector2 *world_faces(void)
{
    return faces;
}

uint8_t *world_heights(void)
{
    return heights;
}

Vector2 world_centre(void)
{
    return centre;
}

Vector2 world_bounds(void)
{
    return bounds;
}

float world_scale(void)
{
    return (float)UV_SCALE;
}

void world_clear(void)
{
    for (size_t i = 0; i < num_tiles; i++) {
        heights[i] = 0;
    }
}

void world_randomise_seismic(uint8_t dh, size_t n)
{
    /* base case return */
    if (n <= 0 || dh <= 0)
        return;

    /* n random seismic events at magnigute dh */
    int angle = 0, r = 0, c = 0;
    Vector2 v = Vector2Zero(), w = Vector2Zero();
    for (size_t i = 0; i < n; i++) {
        angle = GetRandomValue(0, 359);
        r = GetRandomValue(0, num_rows - 1);
        c = GetRandomValue(0, num_cols - 1);
        v = (Vector2) {
        cosf(angle), sinf(angle)};
        w = faces[world_index(r, c)];

        for (size_t j = 0; j < num_tiles; j++) {
            if (Vector2DotProduct(v, Vector2Subtract(faces[j], w)) >= 0) {
                heights[j] += dh;
            }
        }
    }
    world_randomise_seismic(dh / 2, n * 2);
}

void world_renormalise(void)
{
    float max = 0.0f;
    float min = heights[0];
    for (size_t i = 0; i < num_tiles; i++) {
        if (heights[i] > max) {
            max = (float)heights[i];
        }
        if (heights[i] < min) {
            min = (float)heights[i];
        }
    }

    float factor = ((max - min) == 0) ? 0.0f : (MAX_H/(max - min));
    for (size_t i = 0; i < num_tiles; i++) {
        heights[i] = floor((heights[i] - min)*factor);
    }
}

void world_generate(void)
{
    world_clear();
    world_randomise_seismic(8, 8);
    world_renormalise();
}
