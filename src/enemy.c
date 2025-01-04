#include "include/infex.h"

uint8_t enemy[MAX_TILES] = { 0 };

void enemy_set(size_t i, uint8_t val)
{
    enemy[i] = val;
}
