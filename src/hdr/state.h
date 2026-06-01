#ifndef INFEX_STATE_H
#define INFEX_STATE_H

#include "infex.h"

bool state_is_quit(void);
void state_set_quit(bool b);
bool state_is_debug(void);
void state_set_debug(bool b);

bool is_building_shadow(void);
void set_building_shadow(bool b);
enum BUILDING_ID building_shadow_id(void);
void set_building_shadow_id(enum BUILDING_ID b);
enum FOOTPRINT_TYPE building_shadow_footprint(void);
void set_building_shadow_index_fn(size_t (*fn)(void));
size_t building_shadow_index(void);
void building_shadow_reset(void);
Vector2 building_shadow_pos(void);

enum INFEX_SCREEN state_screen(void);
void state_set_screen(enum INFEX_SCREEN s);

void state_initialise();
void state_update(void);
void state_terminate(void);

#endif
