#ifndef INFEX_INTERFACE_H
#define INFEX_INTERFACE_H

void interface_initialise(int width, int height);
void interface_reset(void);
void interface_render(enum INFEX_SCREEN s);
void interface_update(float dt);
void interface_terminate(void);

#endif
