#include <string.h>

#include "include/world.h"

#define ROOT_3 (1.7320508f)
#define ROOT_3_INV (0.5773502f)
#define UV_SCALE (100)
#define DELTA_U (UV_SCALE * ROOT_3)
#define DELTA_V (UV_SCALE / 2.0f)

/* is_null is a guarantee that all the data fields are blank and/or NULLed */
bool is_null = true;

/* basic grid geometry */
size_t num_tiles = 0;
size_t num_rows = 0,
       num_cols = 0;
Vector2 *centres = NULL; /* geometric locations on-screen of tile centres */

/* scalar function map data */
uint8_t *heights = NULL; /* heightmap of tiles */
float *enemy = NULL;     /* heightmap of enemy growth */

void world_initialise(size_t rows, size_t cols)
{
    if (!is_null) return;

    num_rows = rows, num_cols = cols;
    num_tiles = rows * cols;

    heights = malloc(num_tiles * sizeof(uint8_t));
    memset(heights, 0, num_tiles);

    int u = 0, v = 0;
    centres = malloc(num_tiles * sizeof(Vector2));
    Vector2 *ptr = centres;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            *ptr = (Vector2) {u, v};
            u += DELTA_U;
            ptr++;
        }
        v += DELTA_V;
        if (r % 2) {
            u -= DELTA_U;
        }
    }

    is_null = false;
}

bool world_is_null(void)
{
    return is_null;
}

size_t world_num_tiles(void)
{
    return num_tiles;
}

Vector2 *world_centres(void)
{
    return centres;
}

uint8_t *world_heights(void)
{
    return heights;
}
