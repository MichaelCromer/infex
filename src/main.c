#include <raylib.h>

enum INFEX_SCREEN {
    INFEX_SCREEN_NONE,
    INFEX_SCREEN_TITLE,
    INFEX_SCREEN_GAME,
};

struct GameState {
    int screen_width, screen_height;
    enum INFEX_SCREEN screen_curr;
};

void draw_screen_title(struct GameState *state)
{
    DrawRectangle(0, 0, state->screen_width, state->screen_height, GREEN);
    DrawText("Title Screen", 20, 20, 40, DARKGREEN);
}

void draw_screen_game(struct GameState *state)
{
    DrawRectangle(0, 0, state->screen_width, state->screen_height, PURPLE);
    DrawText("In-Game", 20, 20, 40, MAROON);
}

void draw_screen_none(struct GameState *state)
{
    DrawRectangle(0, 0, state->screen_width, state->screen_height, LIGHTGRAY);
    DrawText("NULL Screen", 20, 20, 40, GRAY);
}

void draw_screen(struct GameState *state)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    switch (state->screen_curr) {
        case INFEX_SCREEN_TITLE:
            draw_screen_title(state);
            break;
        case INFEX_SCREEN_GAME:
            draw_screen_game(state);
            break;
        default:
            draw_screen_none(state);
            break;
    }

    EndDrawing();
}

void state_update(struct GameState *state)
{
    switch (state->screen_curr) {
        case INFEX_SCREEN_TITLE:
            if (IsKeyPressed(KEY_ENTER)) {
                state->screen_curr = INFEX_SCREEN_GAME;
            }
            break;
        case INFEX_SCREEN_GAME:
            if (IsKeyPressed(KEY_ENTER)) {
                state->screen_curr = INFEX_SCREEN_TITLE;
            }
            break;
        default:
            break;
    }
}

int main(void)
{
    struct GameState state = {.screen_width = 800,.screen_height = 450,.screen_curr =
            INFEX_SCREEN_TITLE
    };
    InitWindow(state.screen_width, state.screen_height, "Infex");

    while (!WindowShouldClose()) {
        draw_screen(&state);

        state_update(&state);
    }

    CloseWindow();
    return 0;
}
