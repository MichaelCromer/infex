#include "hdr/draw.h"
#include "hdr/state.h"

int main(void)
{
    state_initialise();
    draw_initialise();

    while (!is_quit() && !WindowShouldClose()) { /* WindowShouldClose for debug only */
        state_update();
        draw_screen();
    }

    state_terminate();

    return 0;
}
