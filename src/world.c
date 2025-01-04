#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "include/world.h"

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

/* scalar function map data */
Vector2 faces[MAX_TILES] = { 0 };       /* locations on-screen of tile faces */
Vector2 vertices[MAX_VERTS] = { 0 };    /* locations on-screen of tile vertices */
Color colours[MAX_TILES] = { 0 };       /* colours on-screen of tile faces */
uint8_t heights[MAX_TILES] = { 0 };     /* heightmap of tiles */

/* current state */
size_t num_faces = 0, num_vertices = 0;
size_t num_rows = 0, num_cols = 0;

Vector2 centre = { 0 };
Vector2 bounds = { 0 };         /* lower bounds always 0, upper bounds given here */

void world_initialise(size_t rows, size_t cols)
{
    if ((cols > MAX_C) || (cols > MAX_R)) return;

    /* establish limits and other single constants */
    num_rows = rows, num_cols = cols;
    num_faces = rows * cols;
    num_vertices = 2 * ((num_rows + 1) * (num_cols + 1) - 1);
    bounds = (Vector2) { cols * DELTA_C, rows * DELTA_R };
    centre = (Vector2) { bounds.x / 2.0f, bounds.y / 2.0f };

    float u = 0, v = 0;     /* for current tile face positions */
    size_t i = 0, j = 0;    /* for calculating face and vertex index positions */

    /* calculate the geometric data */
    for (size_t r = 0; r < num_rows; r++) {
        u = (r % 2) ? (DELTA_C / 2) : 0.0f;

        /* first (non-systematic) row of vertices */
        if (r == 0) {
            for (size_t c = 0; c < num_cols; c++) {
                vertices[j++] = (Vector2) { u, v - (2 * DELTA_R / 3) };
                u += DELTA_C;
            }
            u = (r % 2) ? (DELTA_C / 2) : 0.0f;
        }

        /* all the faces and the systematic vertices */
        for (size_t c = 0; c < num_cols; c++) {
            faces[i] = (Vector2) { u, v };
            heights[i] = 0;
            vertices[j++] = (Vector2) { u - (DELTA_C / 2), v - (DELTA_R / 3) };
            vertices[j++] = (Vector2) { u - (DELTA_C / 2), v + (DELTA_R / 3) };
            u += DELTA_C;
            i++;
        }
        vertices[j++] = (Vector2) { u - (DELTA_C / 2), v - (DELTA_R / 3) };
        vertices[j++] = (Vector2) { u - (DELTA_C / 2), v + (DELTA_R / 3) };

        /* last (non-systematic) row of vertices */
        if (r == num_rows - 1) {
            u = (r % 2) ? (DELTA_C / 2) : 0.0f;
            for (size_t c = 0; c < num_cols; c++) {
                vertices[j++] = (Vector2) { u, v + (2 * DELTA_R / 3) };
                u += DELTA_C;
            }
        }
        v += DELTA_R;
    }
}

size_t world_index(size_t r, size_t c)
{
    return (num_cols*r) + c;
}

size_t world_row(size_t i)
{
    return i / num_cols;
}

size_t world_col(size_t i)
{
    return i % num_cols;
}

size_t world_num_faces(void)
{
    return num_faces;
}

size_t world_num_vertices(void)
{
    return num_vertices;
}

size_t world_num_rows(void)
{
    return num_rows;
}

size_t world_num_cols(void)
{
    return num_cols;
}

Vector2 *world_faces(void)
{
    return faces;
}

Vector2 *world_vertices(void)
{
    return vertices;
}

Color *world_colours(void)
{
    return colours;
}

uint8_t *world_heights(void)
{
    return heights;
}

uint8_t world_height_rc(size_t r, size_t c)
{
    return heights[world_index(r, c)];
}

uint8_t world_height(size_t i)
{
    return heights[i];
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
    return (float)TILE_EDGE_LENGTH;
}

void world_clear(void)
{
    for (size_t i = 0; i < num_faces; i++) {
        heights[i] = 0;
    }
}

void world_generate_seismic(uint8_t dh, size_t n)
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

        for (size_t j = 0; j < num_faces; j++) {
            if (Vector2DotProduct(v, Vector2Subtract(faces[j], w)) >= 0) {
                heights[j] += dh;
            }
        }
    }
    world_generate_seismic(dh / 2, n * 2);
}

size_t world_num_neighbours(size_t i)
{
    size_t r = world_row(i);
    size_t c = world_col(i);
    bool odd_row = (r % 2);

    if ((r == 0) || (r == num_rows - 1)) {
        if (c == 0) {
            return (odd_row) ? 3 : 2;
        } else if (c == (num_cols - 1)) {
            return (odd_row) ? 2 : 3;
        } else {
            return 4;
        }
    }

    if (c == 0) {
        return (odd_row) ? 5 : 3;
    } else if (c == (num_cols - 1)) {
        return (odd_row) ? 3 : 5;
    } else {
        return 6;
    }
}

void world_generate_erosion(void)
{
    float buf[MAX_TILES] = { 0 };
    bool odd_row = false;
    size_t i = 0;

    for (size_t r = 0; r < num_rows; r++) {
        odd_row = (r % 2);

        for (size_t c = 0; c < num_cols; c++) {
            buf[i] += world_height(i);

            if (c < (num_cols - 1)) {
                buf[i] += world_height(i + 1);
                buf[i + 1] += world_height(i);
            }

            if (r < (num_rows - 1)) {
                buf[i] += world_height(i + num_cols);
                buf[i + num_cols] += world_height(i);

                if (odd_row && (c < (num_cols - 1))) {
                    buf[i] += world_height(i + num_cols + 1);
                    buf[i + num_cols + 1] += world_height(i);
                }

                if (!odd_row && (c > 0)) {
                    buf[i] += world_height(i + num_cols - 1);
                    buf[i + num_cols - 1] += world_height(i);
                }
            }

            i++;
        }
    }

    for (size_t i = 0; i < num_faces; i++) {
        heights[i] = buf[i] / (1.0f + world_num_neighbours(i));
    }
}

void world_renormalise(void)
{
    float max = 0.0f;
    float min = heights[0];
    for (size_t i = 0; i < num_faces; i++) {
        if (heights[i] > max) {
            max = (float)heights[i];
        }
        if (heights[i] < min) {
            min = (float)heights[i];
        }
    }

    float factor = ((max - min) == 0) ? 0.0f : (MAX_H/(max - min));
    for (size_t i = 0; i < num_faces; i++) {
        heights[i] = floor((heights[i] - min)*factor);
    }
}

void world_calculate_colours(void)
{
    Color c0 = { 49, 163, 84, 255 };
    Color c1 = { 114, 84, 40, 255 };

    for (size_t i = 0; i < num_faces; i++) {
        if (heights[i] == 0) {
            colours[i] = BLUE;
            continue;
        }
        colours[i] = ColorLerp(c0, c1, heights[i] / (1.0f * MAX_H));
    }
}

void world_generate(void)
{
    world_clear();
    world_generate_seismic(8, 8);
    world_generate_erosion();
    world_renormalise();
    world_calculate_colours();
}
