#include <string.h>

#include "include/world.h"

#define ROOT_3 (1.7320508f)
#define ROOT_3_INV (0.5773502f)
#define UV_SCALE (100)

struct World *world_create(size_t rows, size_t cols)
{
    struct World *world = malloc(sizeof(struct World));

    world->rows = rows;
    world->cols = cols;

    world->height = malloc(rows * cols * sizeof(uint8_t));
    memset(world->height, 0, rows * cols);

    world->centre = malloc(rows * cols * sizeof(Vector2));
    int i = 0;
    int u = 0, v = 0;
    float du = UV_SCALE * ROOT_3, dv = UV_SCALE / 2.0f;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            (world->centre)[i] = (Vector2) {
            u, v};
            u += du;
            i++;
        }
        v += dv;
    }

    return world;
}
