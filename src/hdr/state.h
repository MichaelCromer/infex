#ifndef INFEX_STATE_H
#define INFEX_STATE_H

#include "infex.h"

bool state_quit(void);
void state_set_quit(bool b);

enum INFEX_SCREEN screen_curr(void);
unsigned int screen_width(void);
unsigned int screen_height(void);

void state_initialise(void);
void state_update(void);

#endif
