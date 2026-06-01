#include <raylib.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#define RAYGUI_IMPLEMENTATION
#include "./lib/raygui/src/raygui.h"
#include "./lib/raygui/styles/amber/style_amber.h"
#pragma GCC diagnostic pop

#include "hdr/action.h"
#include "hdr/state.h"
#include "hdr/draw.h"


Rectangle rec_translate(Rectangle rec, Vector2 vec)
{
    return (Rectangle) { rec.x + vec.x, rec.y + vec.y, rec.width, rec.height };
}


enum MAINMENU_OPTIONS
{
    MAINMENU_PLAY,
    MAINMENU_LOAD,
    MAINMENU_SETTINGS,
    MAINMENU_CREDITS,
    MAINMENU_QUIT,
    NUM_MAINMENU_OPTIONS,
    MAINMENU_NONE
};


/* Main menu screen */
static struct
{
    enum MAINMENU_OPTIONS choice;
    int pad;
    int gap;

    struct {
        Rectangle rec;
        int pad;
        int gap;
        char *title;

        struct {
            Rectangle rec;
            char *text;
        } button[NUM_MAINMENU_OPTIONS];
    } panel_main;

    struct {
        Rectangle rec;
        char *title;
    } panel_side[NUM_MAINMENU_OPTIONS];

    struct {
        Rectangle rec;
        char *title;
        char *message;
        char *buttons;
        int width;
        int height;
        bool show;
    } panel_quit;

} mainmenu = {
    .choice = MAINMENU_NONE,
    .pad = 30,
    .gap = 30,
    .panel_main.pad = 30,
    .panel_main.gap = 12,
    .panel_main.title = "infex",
    .panel_main.button[MAINMENU_PLAY].text = "play",
    .panel_main.button[MAINMENU_LOAD].text = "load",
    .panel_main.button[MAINMENU_SETTINGS].text = "options",
    .panel_main.button[MAINMENU_CREDITS].text = "credits",
    .panel_main.button[MAINMENU_QUIT].text = "quit",
    .panel_side[MAINMENU_PLAY].title = "play",
    .panel_side[MAINMENU_LOAD].title = "load",
    .panel_side[MAINMENU_SETTINGS].title = "options",
    .panel_side[MAINMENU_CREDITS].title = "credits",
    .panel_side[MAINMENU_QUIT].title = "quit",
    .panel_quit.title = "Quit",
    .panel_quit.message = "Really quit?",
    .panel_quit.buttons = "Yes\nNo",
    .panel_quit.width = 240,
    .panel_quit.height = 180,
    .panel_quit.show = false,
};


void interface_initialise(int width, int height)
{
    GuiLoadStyleAmber();

    Rectangle *rec_main = &(mainmenu.panel_main.rec);
    *rec_main = (Rectangle) {
        mainmenu.pad, mainmenu.pad,
        width/3 - mainmenu.pad - mainmenu.gap/2, (height - 2*mainmenu.pad)
    };

    Rectangle rec_button = (Rectangle) {
        rec_main->x + mainmenu.panel_main.pad,
        rec_main->y + rec_main->height/2,
        rec_main->width - 2*mainmenu.panel_main.pad,
        (rec_main->height/2 - mainmenu.panel_main.pad - (NUM_MAINMENU_OPTIONS - 1)*mainmenu.panel_main.gap) / NUM_MAINMENU_OPTIONS
    };

    for (enum MAINMENU_OPTIONS b = 0; b < NUM_MAINMENU_OPTIONS; b++) {
        mainmenu.panel_main.button[b].rec = rec_translate(
            rec_button, (Vector2) { 0, b*(rec_button.height + mainmenu.panel_main.gap) }
        );
        mainmenu.panel_side[b].rec = (Rectangle) {
            width/3 + mainmenu.gap/2, mainmenu.pad,
            2*width/3 - mainmenu.pad - mainmenu.gap/2, (height - 2*mainmenu.pad)
        };
    }

    mainmenu.panel_quit.rec = (Rectangle) {
        (width - mainmenu.panel_quit.width) / 2,
        (height - mainmenu.panel_quit.height) / 2,
        mainmenu.panel_quit.width,
        mainmenu.panel_quit.height,
    };
}


void interface_reset(void)
{

}


void interface_render_mainmenu(void)
{
    int click;
    GuiPanel(mainmenu.panel_main.rec, mainmenu.panel_main.title);

    for (enum MAINMENU_OPTIONS b = 0; b < NUM_MAINMENU_OPTIONS; b++) {
        click = GuiButton(mainmenu.panel_main.button[b].rec, mainmenu.panel_main.button[b].text);
        if (!click) continue;

        if ((mainmenu.panel_quit.show) || (MAINMENU_QUIT == b)) {
            mainmenu.panel_quit.show = true;
        } else {
            mainmenu.choice = (b == mainmenu.choice) ? MAINMENU_NONE : b;
        }
    }

    switch (mainmenu.choice) {
        case MAINMENU_PLAY:
        case MAINMENU_LOAD:
        case MAINMENU_SETTINGS:
        case MAINMENU_CREDITS:
            GuiGroupBox(
                mainmenu.panel_side[mainmenu.choice].rec, mainmenu.panel_side[mainmenu.choice].title
            );
            break;

        case MAINMENU_QUIT:
        case MAINMENU_NONE:
        default: break;
    }

    if (mainmenu.panel_quit.show) {
        click = GuiMessageBox(
            mainmenu.panel_quit.rec,
            mainmenu.panel_quit.title,
            mainmenu.panel_quit.message,
            mainmenu.panel_quit.buttons
        );
        if (0 == click || 2 == click) mainmenu.panel_quit.show = false;
        if (1 == click) action_quit();
    }
}


void interface_render_game(void)
{

}


void interface_render(enum INFEX_SCREEN s)
{
    switch (s) {

        case INFEX_SCREEN_MAINMENU:
            interface_render_mainmenu();
            break;

        case INFEX_SCREEN_GAME:
            interface_render_game();
            break;

        default:
            break;

    }
}

void interface_update(float dt) { (void)dt; }
void interface_terminate(void) {}
