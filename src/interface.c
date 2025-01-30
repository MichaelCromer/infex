#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <stdlib.h>
#include <stdio.h>

#define CLAY_IMPLEMENTATION
#include "lib/clay/clay.h"
#include "lib/clay/renderers/raylib/clay_renderer_raylib.c"

#include "hdr/action.h"
#include "hdr/infex.h"
#include "hdr/state.h"

#define FONT_ID_TITLE 0
#define FONT_TARGET_TITLE "res/font/SyneMono-Regular.ttf"

/*--------------------------------------------------------------------------------------
 *
 * CLAY INTERNALS
 *
 */


uint64_t memory = 0;
Clay_Arena arena = { 0 };
Clay_Dimensions dimensions = { 0 };

Clay_RenderCommandArray (*interface_renderer)(void) = NULL;
Clay_RenderCommandArray render_commands = { 0 };


void clay_stderr(Clay_ErrorData error_data)
{
    static Clay_ErrorType t = 0;
    static uint64_t call = 0;
    fprintf(stderr, "CLAY: Error %d", error_data.errorType);
    if ((error_data.errorType == t)) {
        fprintf(stderr, " (x%lu) ", call);
    } else {
        call = 0;
    }
    fprintf(stderr, "%s\n", error_data.errorText.chars);
    call++;
    t = error_data.errorType;
}


/*--------------------------------------------------------------------------------------
 *  
 *  GENERIC ELEMENTS
 *
 */


enum BUTTON_ID {
    MENU_BUTTON_NONE,
    MENU_BUTTON_TEST,
    MAIN_BUTTON_PLAY,
    MAIN_BUTTON_SETTINGS,
    MAIN_BUTTON_ABOUT,
    MAIN_BUTTON_EXIT,
    PLAY_BUTTON_TUTORIAL,
    PLAY_BUTTON_CAMPAIGN,
    PLAY_BUTTON_RANDOM,
};


struct Button {
    enum BUTTON_ID id;
    Clay_String label;
    struct ButtonConfig *config;
    void (*on_hover)(Clay_ElementId id, Clay_PointerData mouse, intptr_t data);
    void (*on_click)(void);
};


struct ButtonConfig {
    Clay_LayoutConfig layout;
    Clay_RectangleElementConfig rectangle_up;
    Clay_RectangleElementConfig rectangle_hover;
    Clay_RectangleElementConfig rectangle_down;
    Clay_RectangleElementConfig rectangle_disabled;
    Clay_TextElementConfig text;
};


struct ButtonConfig buttonconfig_mainmenu = {
    .layout = {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(36)},
        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
    },
    .rectangle_up = { .color = { 64, 64, 128, 255 }, .cornerRadius = 6 },
    .rectangle_hover = { .color = { 96, 96, 128, 255 }, .cornerRadius = 6 },
    .rectangle_down = { .color = { 128, 128, 128, 255 }, .cornerRadius = 6 },
    .text = { .fontId = FONT_ID_TITLE, .fontSize = 24, .textColor = { 0, 0, 0, 255 } }
};


struct ButtonConfig buttonconfig_sidepanel = {
    .layout = {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(36)},
        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
    },
    .rectangle_up = { .color = { 64, 64, 128, 255 }, .cornerRadius = 6 },
    .rectangle_hover = { .color = { 96, 96, 128, 255 }, .cornerRadius = 6 },
    .rectangle_down = { .color = { 128, 128, 128, 255 }, .cornerRadius = 6 },
    .text = { .fontId = FONT_ID_TITLE, .fontSize = 24, .textColor = { 0, 0, 0, 255 } }
};


/*--------------------------------------------------------------------------------------
 *  
 *  INTERFACES
 *
 */


/*  MAIN MENU   ***********************************************************************/


enum BUTTON_ID mainmenu_selected = MENU_BUTTON_NONE;
void mainmenu_mouseover(Clay_ElementId id, Clay_PointerData mouse, intptr_t data);

#define NUM_MAINMENU_BUTTONS 4
struct Button mainmenu_buttons[NUM_MAINMENU_BUTTONS] = {
    {
        .id = MAIN_BUTTON_PLAY,
        .label = CLAY_STRING("Play"),
        .config = &buttonconfig_mainmenu,
        .on_hover = mainmenu_mouseover,
        .on_click = NULL
    },
    {
        .id = MAIN_BUTTON_SETTINGS,
        .label = CLAY_STRING("Settings"),
        .config = &buttonconfig_mainmenu,
        .on_hover = mainmenu_mouseover,
        .on_click = NULL
    },
    {
        .id = MAIN_BUTTON_ABOUT,
        .label = CLAY_STRING("About"),
        .config = &buttonconfig_mainmenu,
        .on_hover = mainmenu_mouseover,
        .on_click = NULL
    },
    {
        .id = MAIN_BUTTON_EXIT,
        .label = CLAY_STRING("Exit"),
        .config = &buttonconfig_mainmenu,
        .on_hover = mainmenu_mouseover,
        .on_click = action_quit
    }
};

#define NUM_PLAYCAMPAIGN_BUTTONS 2
struct Button playcampaign_buttons[NUM_PLAYCAMPAIGN_BUTTONS] = {
    {
        .id = PLAY_BUTTON_TUTORIAL,
        .label = CLAY_STRING("Tutorial"),
        .config = &buttonconfig_sidepanel,
        .on_click = NULL,
    },
    {
        .id = PLAY_BUTTON_CAMPAIGN,
        .label = CLAY_STRING("Campaign"),
        .config = &buttonconfig_sidepanel,
        .on_click = NULL,
    }
};

struct Button playrandom_button = {
    .id = PLAY_BUTTON_RANDOM,
    .label = CLAY_STRING("Play Now"),
    .config = &buttonconfig_sidepanel,
    .on_hover = mainmenu_mouseover,
    .on_click = action_start_random_game,
};

Clay_String current_profile = CLAY_STRING("<None>");

void mainmenu_render_title(void)
{
    CLAY(
        CLAY_ID("MainMenuTitle"),
        CLAY_RECTANGLE({ .color = { 0 } }),
        CLAY_LAYOUT({
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60)
            },
            .childAlignment = {
                .x = CLAY_ALIGN_X_CENTER,
                .y = CLAY_ALIGN_Y_CENTER
            }
        })
    ) {
        CLAY_TEXT(
            CLAY_STRING("infex"),
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_TITLE,
                .fontSize = 36,
                .textColor = { 0, 30, 60, 255 }
            })
        );
    }
}

void mainmenu_mouseover(Clay_ElementId clay_id, Clay_PointerData mouse, intptr_t data)
{
    (void)clay_id;
    if (mouse.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) return;

    enum BUTTON_ID id = (enum BUTTON_ID) data;
    if (id == mainmenu_selected) {
        mainmenu_selected = MENU_BUTTON_NONE;
        return;
    }

    for (size_t b = 0; b < NUM_MAINMENU_BUTTONS; b++) {
        if (mainmenu_buttons[b].id != id) continue;
        mainmenu_selected = mainmenu_buttons[b].id;
        if (mainmenu_buttons[b].on_click) mainmenu_buttons[b].on_click();
    }

    if (id == PLAY_BUTTON_RANDOM) playrandom_button.on_click();
}

void mainmenu_render_button(struct Button *button)
{
    CLAY(
        CLAY_LAYOUT(button->config->layout),
        Clay_OnHover(button->on_hover, button->id),
        Clay_Hovered()
            ? CLAY_RECTANGLE(button->config->rectangle_hover)
            : CLAY_RECTANGLE(button->config->rectangle_up)
    )

    {
        CLAY_TEXT(
            button->label,
            CLAY_TEXT_CONFIG(button->config->text)
        );
    }
}

void mainmenu_render_divline(void)
{
    CLAY(
        CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_GROW(0), .height = 6 } }),
        CLAY_RECTANGLE({ .color = { 120, 120, 120, 255 } })
    ) {}
}

void mainmenu_render_subheading(Clay_String title_text)
{
    CLAY(
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0) }
        })
    ) {
        CLAY_TEXT(
            title_text,
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_TITLE,
                .fontSize = 30,
                .textColor = { 0, 30, 60, 255 }
            })
        );
    }
}

void mainmenu_render_playpanel(void)
{
    mainmenu_render_divline();

    mainmenu_render_subheading(CLAY_STRING("Story Mode"));

    CLAY(
        CLAY_ID("MainMenuPlayPanelCampaignBody"),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0) },
            .childGap = 12,
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
        })
    ) {
        CLAY(
            CLAY_ID("MainMenuPlayPanelCampaignButtons"),
            CLAY_LAYOUT({
                .sizing = { .width = CLAY_SIZING_PERCENT(0.5) },
                .padding = CLAY_PADDING_ALL(18),
                .childGap = 12,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            })
        ) {
            for (size_t b = 0; b < NUM_PLAYCAMPAIGN_BUTTONS; b++) {
                mainmenu_render_button(&playcampaign_buttons[b]);
            }
        }

        CLAY(
            CLAY_ID("MainMenuPlayPanelCampaignOptions"),
            CLAY_LAYOUT({
                .sizing = { .width = CLAY_SIZING_PERCENT(0.5) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            })
        ) {
            CLAY(
                CLAY_ID("MainMenuPlayPanelCurrentProfile"),
                CLAY_LAYOUT({
                    .sizing = { .width = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(12),
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .childGap = 12,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                })
            ) {
                CLAY_TEXT(
                    CLAY_STRING("Current Profile:"),
                    CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_TITLE,
                        .fontSize = 24,
                        .textColor = { 0, 30, 60, 255 }
                    })
                );

                CLAY (
                    CLAY_LAYOUT({ 
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                        .padding = CLAY_PADDING_ALL(6)
                    }),
                    CLAY_RECTANGLE({ .color = { 100, 100, 100, 200 } }),
                    Clay_Hovered()
                        ? CLAY_BORDER_OUTSIDE({
                            .width = 3,
                            .color = { 30, 90, 30, 255 }
                        })
                        : CLAY_BORDER_OUTSIDE({
                            .width = 1,
                            .color = { 90, 90, 90, 255 }
                        })
                ) {
                    CLAY_TEXT(
                        current_profile,
                        CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_TITLE,
                            .fontSize = 24,
                            .textColor = { 0, 30, 60, 255 }
                        })
                    );
                }
            }
        }
    }

    mainmenu_render_divline();

    /* dropdowns/sliders/options */

    /* 
     *  map_size slider
     *  difficulty slider
     *  map_region dropdown
     *      presets
     *  map_type dropdown
     *      presets
     *      custom
     *  if custom type show
     *      water toggle y/n
     *      ruggedness slider
     *      ???
     *
     * */

    mainmenu_render_subheading(CLAY_STRING("Random Map"));

    CLAY(
        CLAY_ID("MainMenuPlayPanelRandomMapButtons"),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childGap = 12
        })
    ) {
        CLAY( CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_PERCENT(0.6) } }) ) {}
        mainmenu_render_button(&playrandom_button);
    }

    mainmenu_render_divline();
}

void mainmenu_render_aboutpanel(void)
{
    mainmenu_render_divline();

    mainmenu_render_subheading(CLAY_STRING("Credits"));

    CLAY(
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
        })
    ) {
        CLAY_TEXT(
            CLAY_STRING("\
                code by quaduck \
                "),
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_TITLE,
                .fontSize = 18,
                .textColor = { 0, 30, 60, 255 }
            })
        );
    }

    mainmenu_render_divline();

    mainmenu_render_subheading(CLAY_STRING("Technical"));

    CLAY(
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .childGap = 12,
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
        })
    ) {
        CLAY_TEXT(
            CLAY_STRING("\
                $ cat src/*.c src/hdr/*.h | sed -n /[a-zA-Z]/p | wc -l\n\
                > 1104\
            "),
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_TITLE,
                .fontSize = 18,
                .textColor = { 0, 30, 60, 255 }
            })
        );
        CLAY_TEXT(
            CLAY_STRING("\
                Code : C\n\
                Graphics : raylib (github.com/raysan/raylib)\n\
                Interface : clay (github.com/nicbarker/clay)\n\
            "),
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_TITLE,
                .fontSize = 18,
                .textColor = { 0, 30, 60, 255 }
            })
        );
    }

    mainmenu_render_divline();
}

void mainmenu_render_sidepanel(void)
{
    CLAY(
        CLAY_ID("MainMenuSidePanelContainer"),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = CLAY_PADDING_ALL(24)
        })
    ) {
        CLAY(
            CLAY_ID("MainMenuSidePanel"),
            CLAY_RECTANGLE({ .color = { 100, 100, 100, 200 }, .cornerRadius = 24 }),
            CLAY_LAYOUT({
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_GROW(0)
                },
                .padding = CLAY_PADDING_ALL(24),
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childGap = 12
            })
        )

        {
            switch (mainmenu_selected) {
                case MAIN_BUTTON_PLAY:
                    mainmenu_render_playpanel();
                    break;
                case MAIN_BUTTON_ABOUT:
                    mainmenu_render_aboutpanel();
                    break;
                default:
                    break;
            }
        }
    }
}

Clay_RenderCommandArray interface_renderer_mainmenu()
{
    Clay_BeginLayout();

    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_RECTANGLE({ .color = { 200, 200, 200, 64 } }),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { .left = 24 },
            .layoutDirection = CLAY_LEFT_TO_RIGHT
        })
    )

    {
        CLAY(
            CLAY_ID("MainMenu"),
            CLAY_RECTANGLE({ .color = { 100, 100, 100, 255 } }),
            CLAY_LAYOUT({
                .sizing = {
                    .width = CLAY_SIZING_PERCENT(0.30),
                    .height = CLAY_SIZING_GROW(0),
                },
                .padding = { .left = 24, .right = 24, .bottom = 30 },
                .childGap = 12,
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            })
        )

        {
            CLAY(CLAY_LAYOUT({ .sizing = { .height = CLAY_SIZING_GROW(0) } })) {}
            mainmenu_render_title();
            for (size_t b = 0; b < NUM_MAINMENU_BUTTONS; b++) {
                if (mainmenu_buttons[b].id == mainmenu_selected) {
                    CLAY(
                        CLAY_LAYOUT(mainmenu_buttons[b].config->layout),
                        CLAY_RECTANGLE(mainmenu_buttons[b].config->rectangle_down)
                    ) {
                        CLAY_TEXT(
                            mainmenu_buttons[b].label,
                            CLAY_TEXT_CONFIG(mainmenu_buttons[b].config->text)
                        );
                    }
                } else {
                    mainmenu_render_button(&mainmenu_buttons[b]);
                }
            }
        }

        if (mainmenu_selected != MENU_BUTTON_NONE) {
            mainmenu_render_sidepanel();
        }
    }

    return Clay_EndLayout();
}


/*  IN-GAME     ***********************************************************************/


Clay_RenderCommandArray interface_renderer_ingame(void)
{
    Clay_BeginLayout();

    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_RECTANGLE({ .color = { 200, 200, 200, 64 } }),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { .left = 24 },
            .layoutDirection = CLAY_LEFT_TO_RIGHT
        })
    )

    {

    }

    return Clay_EndLayout();
}


/*--------------------------------------------------------------------------------------
 *
 * INTERFACE MANAGERS
 *
 */


void interface_initialise(void)
{
    /* MEMORY   */
    memory = Clay_MinMemorySize();
    arena = Clay_CreateArenaWithCapacityAndMemory(memory, malloc(memory));
    dimensions = (Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() };

    /*  INIT    */
    Clay_Initialize(
        arena,
        dimensions,
        (Clay_ErrorHandler) { clay_stderr, 0 }
    );

    /* FONTS    */
    Clay_SetMeasureTextFunction(Raylib_MeasureText, 0);
    Raylib_fonts[FONT_ID_TITLE] = (Raylib_Font) {
        .font = LoadFontEx(FONT_TARGET_TITLE, 48, 0, 400),
        .fontId = FONT_ID_TITLE
    };
    SetTextureFilter(Raylib_fonts[FONT_ID_TITLE].font.texture, TEXTURE_FILTER_BILINEAR);
}


Clay_RenderCommandArray interface_renderer_fallback()
{
    Clay_BeginLayout();

    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { 24, 24 },
            .childGap = 16
        })
    ) { }

    return Clay_EndLayout();
}


void interface_update(float dt)
{
    (void)dt;
    dimensions = (Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() };

    Vector2 mouse = GetMousePosition();
    Clay_SetPointerState(
        (Clay_Vector2) { mouse.x, mouse.y },
        IsMouseButtonDown(0)
    );

    switch (screen_curr()) {
        case INFEX_SCREEN_MAINMENU:
            interface_renderer = interface_renderer_mainmenu;
            break;
        case INFEX_SCREEN_GAME:
            interface_renderer = interface_renderer_ingame;
            break;
        default:
            interface_renderer = interface_renderer_fallback;
            break;
    }

    render_commands = interface_renderer();
}


void interface_render(void)
{
    /* TODO threading ? */
    Clay_Raylib_Render(render_commands);
}


void interface_reset(void)
{
    mainmenu_selected = MENU_BUTTON_NONE;
}


#pragma GCC diagnostic pop
