#ifndef INFEX_STATE_H
#define INFEX_STATE_H

#include "infex.h"

bool is_quit(void);
void set_quit(bool b);
bool is_debug(void);
void set_debug(bool b);

bool is_building_shadow(void);
void set_building_shadow(bool b);

enum INFEX_SCREEN screen_curr(void);
void screen_set_curr(enum INFEX_SCREEN s);
int screen_width(void);
int screen_height(void);

void state_initialise(void);
void state_update(void);
void state_terminate(void);

#endif
