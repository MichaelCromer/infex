#ifndef INFEX_H
#define INFEX_H

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/raylib/src/raylib.h"
#include "../lib/raylib/src/raymath.h"

#define INFEX_DEBUG 0

#define DELTA_COL     (48)
#define DELTA_ROW     (24)


enum INFEX_SCREEN {
    INFEX_SCREEN_MAINMENU,
    INFEX_SCREEN_GAME,
    INFEX_SCREEN_NONE
};


enum GRID_DIR {
    DIR_EE,
    DIR_NE,
    DIR_NW,
    DIR_WW,
    DIR_SW,
    DIR_SE,
    NUM_GRID_DIRS,
    DIR_NONE
};


enum BUILDING_ID {
    BUILDING_TEST_1,
    BUILDING_TEST_3,
    BUILDING_COMMAND_CENTRE,
    BUILDING_GENERATOR_POWER,
    BUILDING_GENERATOR_MINERAL,
    BUILDING_GENERATOR_ORGANIC,
    BUILDING_STORAGE_POWER,
    BUILDING_STORAGE_MINERAL,
    BUILDING_STORAGE_ORGANIC,
    NUM_BUILDING_IDS,
    BUILDING_NONE
};


enum FOOTPRINT_TYPE {
    FOOTPRINT_NONE = 0,
    FOOTPRINT_ONE,
    FOOTPRINT_THREE,
    FOOTPRINT_SIX
};


#endif
