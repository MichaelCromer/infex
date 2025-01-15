#include "hdr/draw.h"
#include "hdr/state.h"

int main(void)
{
    state_initialise();

    InitWindow(screen_width(), screen_height(), "Infex");

    while (!WindowShouldClose()) {
        draw_screen();
        state_update();
    }

    CloseWindow();
    return 0;
}
