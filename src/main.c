#include "hdr/draw.h"
#include "hdr/state.h"

int main(void)
{
    InitWindow(screen_width(), screen_height(), "Infex");

    state_initialise();

    while (!WindowShouldClose()) {
        state_update();
        draw_screen();
    }

    CloseWindow();
    return 0;
}
