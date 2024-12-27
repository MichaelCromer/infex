#include <string.h>

#include "include/world.h"

struct World *world_create(size_t rows, size_t cols)
{
    struct World *world = malloc(sizeof(struct World));

    world->rows = rows;
    world->cols = cols;

    world->height = malloc(rows*cols * sizeof(uint8_t));
    memset(world->height, 0, rows*cols);

    return world;
}
