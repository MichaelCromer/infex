#ifndef INFEX_WORLD_H
#define INFEX_WORLD_H

#include <stdint.h>
#include <stdlib.h>

struct World {
    size_t rows, cols;
    uint8_t *height;
};

struct World *world_create(size_t rows, size_t cols);

#endif
