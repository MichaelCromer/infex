#ifndef INFEX_H
#define INFEX_H

#include <raylib.h>
#include <raymath.h>

#include "camera.h"
#include "mouse.h"
#include "world.h"

#define INFEX_DEBUG 0

/* map geometry and limits */
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

enum INFEX_SCREEN {
    INFEX_SCREEN_NONE,
    INFEX_SCREEN_TITLE,
    INFEX_SCREEN_GAME,
};

#endif
