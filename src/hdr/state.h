#ifndef INFEX_STATE_H
#define INFEX_STATE_H

#include "infex.h"

bool is_quit(void);
void set_quit(bool b);

enum INFEX_SCREEN screen_curr(void);
int screen_width(void);
int screen_height(void);

void state_initialise(void);
void state_update(void);
void state_terminate(void);

#endif
