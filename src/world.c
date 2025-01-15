#include "hdr/infex.h"


/***************************************************************************************
 * DEFINES
 */


#define MAX_C (64)
#define MAX_R (64)
#define MAX_H (8)
#define MAX_TILES ((MAX_C) * (MAX_R))
#define MAX_VERTS (2 * ((MAX_R + 1) * (MAX_C + 1) - 1))
#define GRID_SCALE (16)
#define ROOT_3 (1.7320508f)
#define DELTA_C (GRID_SCALE * ROOT_3)
#define TILE_ASPECT_RATIO (1.0f)
#define DELTA_R (3.0f * TILE_ASPECT_RATIO * GRID_SCALE / 2.0f)

#define ENEMY_UPDATE_INTERVAL (0.33f)   /* seconds between enemy updates    */
#define ENEMY_GROW_MAX (0.05f)          /* maximum absolute increase        */
#define ENEMY_GROW_FACTOR (0.01f)       /* fraction to increase by          */
#define ENEMY_GROW_THRESHOLD (0.01f)    /* minimum value for growth         */
#define ENEMY_SPLIT_THRESHOLD (0.90f)   /* minimum delta before split       */
#define ENEMY_LEVEL_THRESHOLD (0.10f)   /* minimum delta before level flow  */


/***************************************************************************************
 * DATA
 */


/* Geometry     */
Vector2 centres[MAX_TILES] = { 0 };     /* locations on-screen of tile centres */
Vector2 vertices[MAX_VERTS] = { 0 };    /* locations on-screen of tile vertices */
Vector2 middle = { 0 };                 /* geometric middle of grid on-screen */
Vector2 bounds = { 0 };                 /* upper bounds of coordinates on-screen */

size_t num_tiles = 0;
size_t num_vertices = 0;
size_t num_rows = 0;
size_t num_cols = 0;

/* Map          */
uint8_t heights[MAX_TILES] = { 0 };
Color colours[MAX_TILES] = { 0 };       /* colours on-screen of tile faces */

/* Enemy        */
float enemy[MAX_TILES] = { 0 };
float e_tick = 0.0f;
float e_score = 0.0f;

/* Player       */

/* Player - Resource */
typedef enum {
    RESOURCE_POWER = 0,
    RESOURCE_MINERAL,
    RESOURCE_ORGANIC,
} RESOURCE;

typedef float Resource[3];

/* Player - Building */
typedef enum {
    BUILDING_NONE,
    BUILDING_COMMAND_CENTRE,
    BUILDING_GENERATOR_POWER,
    BUILDING_GENERATOR_MINERAL,
    BUILDING_GENERATOR_ORGANIC,
    BUILDING_STORAGE_POWER,
    BUILDING_STORAGE_MINERAL,
    BUILDING_STORAGE_ORGANIC
} BUILDING;

typedef struct {
    BUILDING type;
    size_t position;
} Building;

Building *buildings[MAX_TILES];         /* for tracking all owned tiles */

/***************************************************************************************
 * GRID
 */

void grid_initialise(size_t rows, size_t cols)
{
    if ((cols > MAX_C) || (cols > MAX_R)) return;

    /* establish limits and other single constants */
    num_rows = rows; 
    num_cols = cols;
    num_tiles = rows * cols;
    num_vertices = 2 * ((num_rows + 1) * (num_cols + 1) - 1);

    bounds = (Vector2) { cols * DELTA_C, rows * DELTA_R };
    middle = (Vector2) { bounds.x / 2.0f, bounds.y / 2.0f };

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
            centres[i] = (Vector2) { u, v };
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

size_t grid_index(size_t r, size_t c)
{
    return (num_cols*r) + c;
}

size_t grid_row(size_t i)
{
    return i / num_cols;
}

size_t grid_col(size_t i)
{
    return i % num_cols;
}

size_t grid_size(void)
{
    return num_tiles;
}

size_t grid_num_neighbours(size_t r, size_t c)
{
    bool odd_row = (r % 2);

    if ((r == 0) || (r == (num_rows - 1))) {
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

Vector2 *grid_centres(void)
{
    return centres;
}

Vector2 *grid_vertices(void)
{
    return vertices;
}

float grid_scale(void)
{
    return (float)GRID_SCALE;
}


/***************************************************************************************
 * MAP
 */


uint8_t *map_heights(void)
{
    return heights;
}

uint8_t map_height(size_t i)
{
    return heights[i];
}

Color *map_colours(void)
{
    return colours;
}


/***************************************************************************************
 * MAPGEN
 */


void mapgen_seismic(uint8_t dh, size_t n)
{
    /* base case return */
    if (n <= 0 || dh <= 0) return;

    /* n random seismic events at magnigute dh */
    int angle = 0, r = 0, c = 0;
    Vector2 v = Vector2Zero(), w = Vector2Zero();
    for (size_t i = 0; i < n; i++) {
        angle = GetRandomValue(0, 359);
        r = GetRandomValue(0, num_rows - 1);
        c = GetRandomValue(0, num_cols - 1);
        v = (Vector2) {
        cosf(angle), sinf(angle)};
        w = centres[grid_index(r, c)];

        for (size_t j = 0; j < grid_size(); j++) {
            if (Vector2DotProduct(v, Vector2Subtract(centres[j], w)) >= 0) {
                heights[j] += dh;
            }
        }
    }
    mapgen_seismic(dh / 2, n * 2);
}

void mapgen_erosion(void)
{
    float buf[MAX_TILES] = { 0 };
    bool odd_row = false;
    size_t i = 0;

    for (size_t r = 0; r < num_rows; r++) {
        odd_row = (r % 2);

        for (size_t c = 0; c < num_cols; c++) {
            buf[i] += heights[i];

            if (c < (num_cols - 1)) {
                buf[i] += heights[i + 1];
                buf[i + 1] += heights[i];
            }

            if (r < (num_rows - 1)) {
                buf[i] += heights[i + num_cols];
                buf[i + num_cols] += heights[i];

                if (odd_row && (c < (num_cols -1))) {
                    buf[i] += heights[i + num_cols + 1];
                    buf[i + num_cols + 1] += heights[i];
                } else if (!odd_row && (c > 0)) {
                    buf[i] += heights[i + num_cols - 1];
                    buf[i + num_cols - 1] += heights[i];
                }
            }

            i++;
        }
    }

    i = 0;
    for (size_t r = 0; r < num_rows; r++) {
        for (size_t c = 0; c < num_cols; c++) {
            heights[i] = buf[i] / (1.0f + grid_num_neighbours(r, c));
            i++;
        }
    }
}

void mapgen_renormalise(void)
{
    float max = 0.0f;
    float min = heights[0];
    for (size_t i = 0; i < grid_size(); i++) {
        if (heights[i] > max) {
            max = (float)heights[i];
        }
        if (heights[i] < min) {
            min = (float)heights[i];
        }
    }

    float factor = ((max - min) == 0) ? 0.0f : (MAX_H/(max - min));
    for (size_t i = 0; i < grid_size(); i++) {
        heights[i] = floor((heights[i] - min)*factor);
    }
}

void mapgen_colourset(void)
{
    Color c0 = { 49, 163, 84, 255 };
    Color c1 = { 114, 84, 40, 255 };

    for (size_t i = 0; i < grid_size(); i++) {
        if (heights[i] == 0) {
            colours[i] = BLUE;
            continue;
        }
        colours[i] = ColorLerp(c0, c1, heights[i] / (1.0f * MAX_H));
    }
}


/***************************************************************************************
 * ENEMY
 */


float *enemy_state(void)
{
    return enemy;
}

float enemy_score(void)
{
    return e_score;
}

void enemy_set(size_t i, float val)
{
    enemy[i] = val;
}

float enemy_height(size_t i)
{
    return (enemy[i] + heights[i]);
}

float enemy_delta(size_t i, size_t j)
{
    return (enemy_height(i) - enemy_height(j));
}

void enemy_initialise()
{
    for (size_t i = 0; i < grid_size(); i++) {
        enemy[i] = 0.0f;
    }
    enemy_set(0, 0.1f); /* TODO this is just for testing */
}

void enemy_grow(size_t i)
{
    if (enemy[i] < ENEMY_GROW_THRESHOLD) {
        enemy[i] /= 2;
        return;
    }

    float growth = (ENEMY_GROW_FACTOR * enemy[i]);
    enemy[i] += (growth < ENEMY_GROW_MAX) ? growth : ENEMY_GROW_MAX;
}

/* enemy balances out existing tiles
 * enemy splits to vacant tiles 
 * */
void enemy_flow(size_t i, size_t j)
{
    /* early exit if nothing to do */
    if (FloatEquals(enemy[i], 0.0f) && FloatEquals(enemy[j], 0.0f)) return;

    /* figure out the relevant flow/split case */
    float threshold = (FloatEquals(enemy[i], 0.0f) || FloatEquals(enemy[j], 0.0f))
        ? ENEMY_SPLIT_THRESHOLD
        : ENEMY_LEVEL_THRESHOLD;

    /* get the actual diff in height and early exit if need to */
    float dh = enemy_delta(i, j);
    if (dh < threshold) return;

    /* figure out the direction of flow */
    size_t src = (dh > 0) ? i : j;
    size_t snk = (src == j) ? i : j;

    /* do the flow, accounting for case of not having enough to supply all of dh */
    dh = (dh < enemy[src]) ? dh : enemy[src];
    enemy[src] -= dh / 2.0f;
    enemy[snk] += dh / 2.0f;
}

void enemy_update(void)
{
    e_score = 0.0f;
    size_t i = 0;
    for (size_t r = 0; r < num_rows; r++) {
        for (size_t c = 0; c < num_cols; c++) {
            enemy_grow(i);
            e_score += enemy[i];

            if (c > 0) {
                enemy_flow(i, i - 1);
            }

            if (c < (num_cols - 1)) {
                enemy_flow(i, i + 1);
            }

            if (r > 0) {
                enemy_flow(i, i - num_cols);
            }

            if (r < (num_rows - 1)) {
                enemy_flow(i, i + num_cols);
            }

            i++;
        }
    }
}


/***************************************************************************************
 * WORLD
 */


void world_initialise(size_t rows, size_t cols)
{
    grid_initialise(rows, cols);
}


void world_clear(void)
{
    for (size_t i = 0; i < grid_size(); i++) {
        heights[i] = 0;
        enemy[i] = 0;
    }
}

Vector2 world_middle(void)
{
    return middle;
}

Vector2 world_bounds(void)
{
    return bounds;
}

void world_generate(void)
{
    world_clear();
    mapgen_seismic(8, 8);
    mapgen_erosion();
    mapgen_renormalise();
    mapgen_colourset();
    enemy_initialise();
}

void world_update(float dt)
{
    e_tick += dt;
    if (e_tick > ENEMY_UPDATE_INTERVAL) {
        enemy_update();
        e_tick -= ENEMY_UPDATE_INTERVAL;
    }
}
