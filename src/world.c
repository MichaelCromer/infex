#include <math.h>
#include <string.h>

#include "include/world.h"

#define ROOT3 (1.7320508f)
#define ROOT3_INV (0.5773502f)

struct World *world_create(size_t rows, size_t cols)
{
    struct World *world = malloc(sizeof(struct World));

    world->rows = rows;
    world->cols = cols;

    world->height = malloc(rows*cols * sizeof(uint8_t));
    memset(world->height, 0, rows*cols);

    return world;
}
