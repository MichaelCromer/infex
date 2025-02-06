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

#define DELTA_COL     (48)
#define DELTA_ROW     (24)

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


#define NUM_BUILDING_IDS 10
enum BUILDING_ID {
    BUILDING_NONE = 0,
    BUILDING_TEST_1,
    BUILDING_TEST_3,
    BUILDING_COMMAND_CENTRE,
    BUILDING_GENERATOR_POWER,
    BUILDING_GENERATOR_MINERAL,
    BUILDING_GENERATOR_ORGANIC,
    BUILDING_STORAGE_POWER,
    BUILDING_STORAGE_MINERAL,
    BUILDING_STORAGE_ORGANIC
};


enum FOOTPRINT_TYPE {
    FOOTPRINT_NONE = 0,
    FOOTPRINT_ONE,
    FOOTPRINT_THREE,
    FOOTPRINT_SIX
};

enum RESOURCE {
    RESOURCE_POWER = 0,
    RESOURCE_MINERAL,
    RESOURCE_ORGANIC,
};

#endif
