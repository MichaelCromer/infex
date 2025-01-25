#ifndef INFEX_H
#define INFEX_H

#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define INFEX_DEBUG 0

enum INFEX_SCREEN {
    INFEX_SCREEN_NONE,
    INFEX_SCREEN_MAINMENU,
    INFEX_SCREEN_GAME,
};

#define NUM_GRID_DIRS 6
enum GRID_DIR {
    DIR_EE,
    DIR_NE,
    DIR_NW,
    DIR_WW,
    DIR_SW,
    DIR_SE
};

#endif
