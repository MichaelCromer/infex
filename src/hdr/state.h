#ifndef INFEX_STATE_H
#define INFEX_STATE_H

#include "infex.h"

bool is_quit(void);
void set_quit(bool b);
bool is_debug(void);
void set_debug(bool b);

bool is_building_shadow(void);
void set_building_shadow(bool b);
enum BUILDING_ID building_shadow_id(void);
void set_building_shadow_id(enum BUILDING_ID b);
enum FOOTPRINT_TYPE building_shadow_footprint(void);
void set_building_shadow_index_fn(size_t (*fn)(void));
size_t building_shadow_index(void);
void building_shadow_reset(void);
Vector2 building_shadow_pos(void);

enum INFEX_SCREEN screen_curr(void);
void screen_set_curr(enum INFEX_SCREEN s);
int screen_width(void);
int screen_height(void);

void state_initialise(void);
void state_update(void);
void state_terminate(void);

#endif
