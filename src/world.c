/***************************************************************************************
 *  INFEX WORLD GENERATION AND MAP MANAGEMENT
 ***************************************************************************************
 *
 *  All the world's a page
 *
 */

#include "hdr/infex.h"
#include "hdr/state.h"


/*--------------------------------------------------------------------------------------
 *
 *  DEFINES
 *
 */

#define MAX_COL         (64)
#define MAX_ROW         (64)
#define MAX_HEIGHT      (8)
#define MAX_TILES       ((MAX_COL) * (MAX_ROW))
#define MAX_VERTS       (2 * (MAX_ROW - 1) * (MAX_COL - 1))

#define MAX_BUILDINGS               (MAX_TILES / 6)
#define MAX_BUILDING_NEIGHBOURS     (6)

#define ENEMY_UPDATE_INTERVAL   (0.33f) /* seconds between enemy updates    */
#define ENEMY_GROW_MAX          (0.05f) /* maximum absolute increase        */
#define ENEMY_GROW_FACTOR       (0.01f) /* fraction to increase by          */
#define ENEMY_GROW_THRESHOLD    (0.01f) /* minimum value for growth         */
#define ENEMY_SPLIT_THRESHOLD   (0.66f) /* minimum delta before split       */
#define ENEMY_LEVEL_THRESHOLD   (0.05f) /* minimum delta before level flow  */


/*--------------------------------------------------------------------------------------
 *
 *  GEOMETRY
 *
 */

/* Geometry     */
Vector2 faces[MAX_TILES] = { 0 };       /* locations on-screen of tile faces */
Vector2 vertices[MAX_VERTS] = { 0 };    /* locations on-screen of tile vertices */
Vector2 centre = { 0 };                 /* geometric centre of grid on-screen */
Vector2 bounds = { 0 };                 /* lower right map corner */

/* vector steps separating tile corners from tile centres */
Vector2 delta_vert_se = { DELTA_COL / 2.0f, DELTA_ROW / 3.0f };
Vector2 delta_vert_ss = { 0.0f, 2.0f * DELTA_ROW / 3.0f + 2.0f };

uint16_t num_faces = 0;
uint16_t num_vertices = 0;
uint16_t num_rows = 0;
uint16_t num_cols = 0;

/* Map          */
uint8_t heights[MAX_TILES] = { 0 };
Color colours[MAX_TILES] = { 0 };       /* colours on-screen of tile faces */
uint8_t slopes[MAX_VERTS] = { 0 };      /* bitmask showing edge types at verts */

/* Enemy        */
float enemy[MAX_TILES] = { 0 };
float e_tick = 0.0f;
float e_score = 0.0f;

/* Player       */
Vector3 p_res_network = { 0 };  /* currently in-network         */
Vector3 p_res_surplus = { 0 };  /* (generation - consumption)   */
Vector3 p_res_current = { 0 };  /* currently in storage         */
Vector3 p_res_storage = { 0 };  /* storage maximum              */

Vector3 b_res_internal[MAX_BUILDINGS] = { 0 };  /* available for use in buildings   */
Vector3 b_res_external[MAX_BUILDINGS] = { 0 };  /* passing via buildings            */
Vector3 b_res_upkeep[MAX_BUILDINGS] = { 0 };    /* required per tick                */
Vector2 b_positions[MAX_BUILDINGS] = { 0 };     /* vector position on screen        */
uint16_t b_footprints[MAX_TILES] = { 0 };       /* which grid tiles hit which bldgs */
uint8_t b_texture_ids[MAX_BUILDINGS] = { 0 };                  /* which texture to use for each    */
uint8_t b_hitpoints_current[MAX_BUILDINGS] = { 0 };            /* hp                               */
uint8_t b_hitpoints_max[MAX_BUILDINGS] = { 0 };                /* max hp                           */

/* buildings managed via swapback with NULLish first element */
uint16_t b_next = 1;

/*--------------------------------------------------------------------------------------
 *
 *  GRID
 *
 */


uint16_t grid_face_index(uint16_t r, uint16_t c) { return num_cols*r + c; }
uint16_t grid_vert_index(uint16_t r, uint16_t c) { return 2*(num_cols - 1)*r + c; }
uint16_t grid_row(uint16_t i) { return i / num_cols; }
uint16_t grid_col(uint16_t i) { return i % num_cols; }
uint16_t grid_num_faces(void) { return num_faces; }
uint16_t grid_num_vertices(void) { return num_vertices; }
uint16_t grid_num_rows(void) { return num_rows; }
uint16_t grid_num_cols(void) { return num_cols; }
Vector2 *grid_faces(void) { return faces; }
Vector2 grid_face(uint16_t i) { return faces[i]; }
Vector2 *grid_vertices(void) { return vertices; }
Vector2 grid_vert(uint16_t i) { return vertices[i]; }


void grid_initialise(uint16_t rows, uint16_t cols)
{
    if ((cols > MAX_COL) || (cols > MAX_ROW)) return;

    /* establish limits and other single constants */

    /* centre of world is translated to centre of screen in pixel land */
    bounds = (Vector2) { (cols - 0.5f) * DELTA_COL, (rows - 1) * DELTA_ROW };
    centre = Vector2Scale(bounds, 0.5f);

    num_rows = rows;
    num_cols = cols;
    num_faces = rows * cols;
    num_vertices = 2 * (rows - 1) * (cols - 1);

    float u = 0, v = 0;     /* for current tile face positions */
    uint16_t i = 0, j = 0;    /* for calculating face and vertex index positions */
    bool odd_row = false;

    /* calculate the geometric tile data */
    for (uint16_t r = 0; r < num_rows; r++) {
        odd_row = (r % 2);
        u = ((odd_row) ? (DELTA_COL / 2) : 0.0f);

        for (uint16_t c = 0; c < num_cols; c++) {
            /* faces are easy; they're 1:1 with (r,c) pairs */
            faces[i++] = (Vector2) { u, v };
            u += DELTA_COL;

            /* vertices are harder, they are either 0:1, 1:1, or 2:1 with (r,c)s */
            /* canonically take tile i as owning verts ss and se of it */
            if (r == (num_rows - 1)) continue; /* no verts last row */
            if ((c > 0) && (c < (num_cols - 1))) {
                vertices[j++] = Vector2Add(faces[i-1], delta_vert_ss);
                vertices[j++] = Vector2Add(faces[i-1], delta_vert_se);
            } else if (c == 0) {
                if (odd_row) vertices[j++] = Vector2Add(faces[i-1], delta_vert_ss);
                vertices[j++] = Vector2Add(faces[i-1], delta_vert_se);
            } else if ((c == (num_cols - 1))) {
                if (!odd_row)  vertices[j++] = Vector2Add(faces[i-1], delta_vert_ss);
            }
        }
        v += DELTA_ROW;
    }
}

uint16_t grid_num_neighbours(uint16_t r, uint16_t c)
{
    bool odd_row = (r % 2);

    if ((r == 0) || (r == (num_rows - 1))) {
        if (c == 0) return (odd_row) ? 3 : 2;
        if (c == (num_cols - 1)) return (odd_row) ? 2 : 3;
        return 4;
    }

    if (c == 0) return (odd_row) ? 5 : 3;
    if (c == (num_cols - 1)) return (odd_row) ? 3 : 5;
    return 6;
}

/*--------------------------------------------------------------------------------------
 *
 *  MAP
 *
 */

uint8_t *map_slopes(void) { return slopes; }
Color *map_colours(void) { return colours; }


void map_gen_seismic(uint8_t dh, uint16_t n)
{
    /* base case return */
    if (n <= 0 || dh <= 0) return;

    /* n random seismic events at magnigute dh */
    int angle = 0;
    Vector2 v = Vector2Zero(), w = Vector2Zero();
    for (uint16_t i = 0; i < n; i++) {
        angle = GetRandomValue(0, 359);
        v = (Vector2) { cosf(angle), sinf(angle)};
        w = faces[GetRandomValue(0, num_faces - 1)];

        for (uint16_t j = 0; j < num_faces; j++) {
            if (Vector2DotProduct(v, Vector2Subtract(faces[j], w)) >= 0) {
                heights[j] += dh;
            }
        }
    }
    map_gen_seismic(dh / 2, n * 2);
}


void map_gen_erosion(uint16_t rounds)
{
    if (!rounds) return;

    float buf[MAX_TILES] = { 0 };
    bool odd_row = false;
    uint16_t i = 0;

    for (uint16_t r = 0; r < num_rows; r++) {
        odd_row = (r % 2);

        for (uint16_t c = 0; c < num_cols; c++) {
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
                }

                if (!odd_row && (c > 0)) {
                    buf[i] += heights[i + num_cols - 1];
                    buf[i + num_cols - 1] += heights[i];
                }
            }

            i++;
        }
    }

    i = 0;
    for (uint16_t r = 0; r < num_rows; r++) {
        for (uint16_t c = 0; c < num_cols; c++) {
            heights[i] = buf[i] / (1.0f + grid_num_neighbours(r, c));
            i++;
        }
    }

    map_gen_erosion(rounds - 1);
}


void map_gen_renormalise(void)
{
    float max = 0.0f;
    float min = heights[0];
    for (uint16_t i = 0; i < num_faces; i++) {
        if (heights[i] > max) {
            max = (float)heights[i];
        }
        if (heights[i] < min) {
            min = (float)heights[i];
        }
    }

    float factor = ((max - min) == 0) ? 0.0f : (MAX_HEIGHT/(max - min));
    for (uint16_t i = 0; i < num_faces; i++) {
        heights[i] = floor((heights[i] - min)*factor);
    }

    uint16_t h = 0, i = 0, j = 0, k = 0;
    uint16_t m = 0;
    bool done = false;
    while (!done) {
        done = true;
        for (uint16_t r = 0; r < num_rows; r++) {
            i = r * num_cols;
            h = i - num_cols + ((r % 2) ? 1 : 0);
            j = i + 1;
            k = i + num_cols + ((r % 2) ? 1 : 0);
            for (uint16_t c = 0; c < num_cols - 1; c++) {
                if ((heights[i] != heights[j])
                    && (heights[j] != heights[k])
                    && (heights[k] != heights[i])
                    && (r < num_rows - 1)
                ) {
                    m = (heights[i] > heights[j]) ? j : i;
                    m = (heights[k] > heights[m]) ? m : k;
                    heights[m]++;
                    done = false;
                }
                if ((heights[i] != heights[j])
                    && (heights[j] != heights[h])
                    && (heights[h] != heights[i])
                    && (r > 0)
                ) {
                    m = (heights[i] > heights[j]) ? j : i;
                    m = (heights[h] > heights[m]) ? m : h;
                    heights[m]++;
                    done = false;
                }
                h++;
                i++;
                j++;
                k++;
            }
        }
    }
}


void map_gen_colours(void)
{
    Color c0 = { 49, 163, 84, 255 };
    Color c1 = { 114, 84, 40, 255 };

    for (uint16_t i = 0; i < num_faces; i++) {
        if (heights[i] == 0) {
            colours[i] = BLUE;
            continue;
        }
        colours[i] = ColorLerp(c0, c1, heights[i] / (1.0f * MAX_HEIGHT));
    }
}


void map_gen_slopes(void)
{
    uint16_t u = 0, v = 0, h = 0, i = 0, j = 0, k = 0;
    for (uint16_t r = 0; r < num_rows; r++) {
        i = r * num_cols;
        h = i - num_cols + ((r % 2) ? 1 : 0);
        j = i + 1;
        k = h + 2*num_cols;
        u = 2 * r * (num_cols - 1) + ((r % 2) ? 1 : 0);
        v = u - 2*(num_cols - 1);
        for (uint16_t c = 0; c < num_cols - 1; c++) {
            if (r > 0) {
                slopes[v] = 0;
                if (heights[j] == heights[h]) {
                    if (heights[i] < heights[j]) {
                        slopes[v] = 7;
                    } else if (heights[i] > heights[j]) {
                        slopes[v] = 10;
                    }
                } else if (heights[i] == heights[j]) {
                    if (heights[h] < heights[i]) {
                        slopes[v] = 8;
                    } else if (heights[h] > heights[i]) {
                        slopes[v] = 11;
                    }
                } else {
                    if (heights[j] < heights[h]) {
                        slopes[v] = 9;
                    } else if (heights[j] > heights[h]) {
                        slopes[v] = 12;
                    }
                }
                v += 2;
            }

            if (r < (num_rows - 1)) {
                slopes[u] = 0;
                if (heights[j] == heights[k]) {
                    if (heights[i] < heights[j]) {
                        slopes[u] = 1;
                    } else if (heights[i] > heights[j]) {
                        slopes[u] = 4;
                    }
                } else if (heights[i] == heights[j]) {
                    if (heights[k] < heights[i]) {
                        slopes[u] = 2;
                    } else if (heights[k] > heights[i]) {
                        slopes[u] = 5;
                    }
                } else {
                    if (heights[j] < heights[k]) {
                        slopes[u] = 3;
                    } else if (heights[j] > heights[k]) {
                        slopes[u] = 6;
                    }
                }
                u += 2;
            }

            h++, i++, j++, k++;
        }
    }
}


/*--------------------------------------------------------------------------------------
 *
 *  ENEMY
 *
 */


float *enemy_state(void)
{
    return enemy;
}

float enemy_score(void)
{
    return e_score;
}

void enemy_set(uint16_t i, float val)
{
    enemy[i] = val;
}

float enemy_height(uint16_t i)
{
    return (enemy[i] + heights[i]);
}

float enemy_delta(uint16_t i, uint16_t j)
{
    return (enemy_height(i) - enemy_height(j));
}

void enemy_initialise()
{
    for (uint16_t i = 0; i < num_faces; i++) {
        enemy[i] = 0.0f;
    }
    enemy_set(GetRandomValue(0, num_faces - 1), 1.0f); /* TODO this is just for testing */
}

void enemy_grow(uint16_t i)
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
void enemy_flow(uint16_t i, uint16_t j)
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
    uint16_t src = (dh > 0) ? i : j;
    uint16_t snk = (src == j) ? i : j;

    /* do the flow, accounting for case of not having enough to supply all of dh */
    dh = (dh < enemy[src]) ? dh : enemy[src];
    enemy[src] -= dh / 2.0f;
    enemy[snk] += dh / 2.0f;
}

void enemy_update(void)
{
    e_score = 0.0f;
    uint16_t i = 0;
    for (uint16_t r = 0; r < num_rows; r++) {
        for (uint16_t c = 0; c < num_cols; c++) {
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


/*--------------------------------------------------------------------------------------
 *
 *  PLAYER
 *
 */

uint16_t player_num_buildings(void) { return b_next - 1; }


/*--/   BUILDING    /-----------------------------------------------------------------*/

uint8_t *building_textures(void) { return b_texture_ids; }
Vector2 *building_positions(void) { return b_positions; }


enum FOOTPRINT_TYPE building_footprint_type(enum BUILDING_ID id)
{
    switch (id) {
        case BUILDING_TEST_1:
            return FOOTPRINT_ONE;
        case BUILDING_TEST_3:
            return FOOTPRINT_THREE;
        default:
            return FOOTPRINT_NONE;
    }
}


void building_create(const enum BUILDING_ID id, uint16_t grid_index)
{
    b_texture_ids[b_next] = id;
    b_positions[b_next] = faces[grid_index];

    b_next++;
}


/*--------------------------------------------------------------------------------------
 *
 * WORLD
 *
 */

Vector2 world_centre(void) { return centre; }
Vector2 world_bounds(void) { return bounds; }

void world_initialise(uint16_t rows, uint16_t cols)
{
    grid_initialise(rows, cols);
}


void world_clear(void)
{
    e_score = 0;
    b_next = 1;
    for (uint16_t i = 0; i < num_faces; i++) {
        heights[i] = 0;
        slopes[i] = 0;
        enemy[i] = 0;
    }
}

void world_generate(void)
{
    world_clear();
    map_gen_seismic(8, 8);
    map_gen_erosion(2);
    map_gen_renormalise();
    map_gen_colours();
    map_gen_slopes();
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
