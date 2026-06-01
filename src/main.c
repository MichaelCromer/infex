#include "hdr/draw.h"
#include "hdr/infex.h"
#include "hdr/state.h"


void initialise(void)
{
    InitWindow(0, 0, "Infex");
    int width = GetScreenWidth(), height = GetScreenHeight();

    state_initialise();
    draw_initialise(width, height);
}


void update(void)
{
    state_update();
    draw_state();
}


void terminate(void)
{
    draw_terminate();
    state_terminate();
    CloseWindow();
}


int main(void)
{
    initialise();

    while (!state_is_quit() && !WindowShouldClose()) { /* WindowShouldClose for debug only */
        update();
    }

    terminate();

    return 0;
}
