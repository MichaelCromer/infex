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


/*  APPEARANCE  ***********************************************************************/


const Clay_Color colour_black = { 0, 0, 0, 255 };
const Clay_Color colour_haze = { 200, 200, 200, 64 };

const Clay_Color colour_panel_menu = { 100, 100, 100, 255 };
const Clay_Color colour_panel_menu_accent = { 120, 120, 120, 255 };
const Clay_Color colour_panel_primary = { 60, 60, 180, 255 };
const Clay_Color colour_panel_primary_accent = { 24, 24, 96, 255 };

const Clay_Color colour_text_header = { 0, 30, 60, 255 };
const Clay_Color colour_text_subheader = { 0, 30, 60, 255 };
const Clay_Color colour_text_body = { 0, 30, 60, 255 };
const Clay_Color colour_text_label = { 0, 0, 0, 255 };

const Clay_Color colour_button_primary_up = { 64, 64, 128, 255 };
const Clay_Color colour_button_primary_hover = { 96, 96, 128, 255 };
const Clay_Color colour_button_primary_down = { 128, 128, 128, 255 };

const Clay_Color colour_button_menu_up = { 64, 64, 128, 255 };
const Clay_Color colour_button_menu_hover = { 96, 96, 128, 255 };
const Clay_Color colour_button_menu_down = { 128, 128, 128, 255 };

/*  BUTTONS     ***********************************************************************/


enum BUTTON_ID {
    BUTTON_NONE = 0,
    BUTTON_TEST,
    MAIN_BUTTON_PLAY,
    MAIN_BUTTON_SETTINGS,
    MAIN_BUTTON_ABOUT,
    MAIN_BUTTON_EXIT,
    PLAY_BUTTON_TUTORIAL,
    PLAY_BUTTON_CAMPAIGN,
    PLAY_BUTTON_RANDOM,
    BOTTOMBAR_BUTTON_BUILD1,
    BOTTOMBAR_BUTTON_BUILD2,
};


struct Button {
    enum BUTTON_ID id;
    bool down;
    Clay_String label;
    void (*on_hover)(Clay_ElementId id, Clay_PointerData mouse, intptr_t data);
    void (*on_click)(void);
};


/*
 *  ButtonConfig
 *
 *  Holds enough Clay config structs to describe all the button states
 *  Allows for convenient classes of button
 */
struct ButtonConfig {
    Clay_LayoutConfig layout;
    Clay_RectangleElementConfig rectangle_up;
    Clay_RectangleElementConfig rectangle_hover;
    Clay_RectangleElementConfig rectangle_down;
    Clay_RectangleElementConfig rectangle_disabled;
    Clay_TextElementConfig text;
};


struct ButtonArray {
    enum BUTTON_ID selected;
    size_t length;
    struct Button *buttons;
    struct ButtonConfig *buttonconfig;
    Clay_LayoutConfig layout;
};


/*
 *  Menu Button Config
 *      - Title screen main menu
 *      - Title screen side panels
 */
struct ButtonConfig buttonconfig_menu = {
    .layout = {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(36)},
        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
    },
    .rectangle_up = { .color = colour_button_menu_up, .cornerRadius = 6 },
    .rectangle_hover = { .color = colour_button_menu_hover, .cornerRadius = 6 },
    .rectangle_down = { .color = colour_button_menu_down, .cornerRadius = 6 },
    .text = { .fontId = FONT_ID_TITLE, .fontSize = 24, .textColor = colour_text_label }
};


/*  Primary Button Config
 *      - In-game buttons (top/bottom bar)
 */
struct ButtonConfig buttonconfig_primary = {
    .layout = {
        .sizing = { .width = CLAY_SIZING_FIXED(60), .height = CLAY_SIZING_PERCENT(0.9)},
        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
    },
    .rectangle_up = { .color = colour_button_primary_up, .cornerRadius = 6 },
    .rectangle_hover = { .color = colour_button_primary_hover, .cornerRadius = 6 },
    .rectangle_down = { .color = colour_button_primary_down, .cornerRadius = 6 },
    .text = { .fontId = FONT_ID_TITLE, .fontSize = 18, .textColor = colour_text_label }
};


void button_render(struct Button *button, struct ButtonConfig *config)
{
    CLAY(
        CLAY_LAYOUT(config->layout),
        Clay_OnHover(button->on_hover, button->id),
        (button->down)
            ? CLAY_RECTANGLE(config->rectangle_down)
            : (
                Clay_Hovered()
                    ? CLAY_RECTANGLE(config->rectangle_hover)
                    : CLAY_RECTANGLE(config->rectangle_up)
            )
    ) {
        CLAY_TEXT(
            button->label,
            CLAY_TEXT_CONFIG(config->text)
        );
    }
}


void buttonarray_render(struct ButtonArray *buttonarray)
{
    CLAY(
        CLAY_LAYOUT(buttonarray->layout)
    ) {
        for (size_t i = 0; i < buttonarray->length; i++) {
            button_render(&(buttonarray->buttons[i]), buttonarray->buttonconfig);
        }
    }
}


void button_click(struct Button *button)
{
    button->down = !button->down;
    if ((button->down) && (button->on_click)) button->on_click();
}


/*
 *  ButtonArrays handle mutually exclusive options
 *  Only one can be active at a time
 */
void buttonarray_click(struct ButtonArray *buttonarray, enum BUTTON_ID clicked)
{
    buttonarray->selected = BUTTON_NONE;
    for (size_t i = 0; i < buttonarray->length; i++) {
        if (buttonarray->buttons[i].down) {
            buttonarray->buttons[i].down = false;
            if (buttonarray->buttons[i].id == clicked) continue;
        }
        if (buttonarray->buttons[i].id != clicked) continue;
        buttonarray->selected = buttonarray->buttons[i].id;
        button_click(&(buttonarray->buttons[i]));
    }
}


const struct Button button_test = {
    .id = BUTTON_TEST,
    .label = CLAY_STRING("Test"),
};


/*--------------------------------------------------------------------------------------
 *
 *  INTERFACES
 *
 */


/*  MAIN MENU   ***********************************************************************/


void mainmenu_hover(Clay_ElementId id, Clay_PointerData mouse, intptr_t data);

#define NUM_MAINMENU_BUTTONS 4
struct Button mainmenu_buttons[NUM_MAINMENU_BUTTONS] = {
    {
        .id = MAIN_BUTTON_PLAY,
        .label = CLAY_STRING("Play"),
        .on_hover = mainmenu_hover,
    },
    {
        .id = MAIN_BUTTON_SETTINGS,
        .label = CLAY_STRING("Settings"),
        .on_hover = mainmenu_hover,
    },
    {
        .id = MAIN_BUTTON_ABOUT,
        .label = CLAY_STRING("About"),
        .on_hover = mainmenu_hover,
    },
    {
        .id = MAIN_BUTTON_EXIT,
        .label = CLAY_STRING("Exit"),
        .on_hover = mainmenu_hover,
    }
};

struct ButtonArray mainmenu_buttonarray = {
    .selected = BUTTON_NONE,
    .length = NUM_MAINMENU_BUTTONS,
    .buttons = mainmenu_buttons,
    .buttonconfig = &buttonconfig_menu,
    .layout = (Clay_LayoutConfig) {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
        .padding = { .top = 24, .bottom = 24, .left = 24, .right = 24 },
        .childGap = (12),
        .layoutDirection = CLAY_TOP_TO_BOTTOM
    }
};

struct Button playbutton_tutorial = {
    .id = PLAY_BUTTON_TUTORIAL,
    .label = CLAY_STRING("Tutorial"),
};

struct Button playbutton_campaign = {
        .id = PLAY_BUTTON_CAMPAIGN,
        .label = CLAY_STRING("Campaign"),
};

struct Button playbutton_random = {
    .id = PLAY_BUTTON_RANDOM,
    .label = CLAY_STRING("Play Now"),
};

Clay_String current_profile = CLAY_STRING("<None>");

void mainmenu_render_title(void)
{
    CLAY(
        CLAY_ID("MainMenuTitle"),
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
                .textColor = colour_text_header
            })
        );
    }
}

void mainmenu_hover(Clay_ElementId clay_id, Clay_PointerData mouse, intptr_t data)
{
    (void)clay_id;
    if (mouse.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) return;
    buttonarray_click(&mainmenu_buttonarray, (enum BUTTON_ID) data);
}


void mainmenu_render_divline(void)
{
    CLAY(
        CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_GROW(0), .height = 6 } }),
        CLAY_RECTANGLE({ .color =  colour_panel_menu_accent })
    ) {}
}


void mainmenu_render_subheader(Clay_String title_text)
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
                .textColor = colour_text_subheader
            })
        );
    }
}


void mainmenu_render_playpanel(void)
{
    mainmenu_render_divline();

    mainmenu_render_subheader(CLAY_STRING("Story Mode"));

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
            })
        ) {
            button_render(&playbutton_tutorial, &buttonconfig_menu);
            button_render(&playbutton_campaign, &buttonconfig_menu);
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
                        .textColor = colour_text_body
                    })
                );

                CLAY (
                    CLAY_LAYOUT({
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                        .padding = CLAY_PADDING_ALL(6)
                    }),
                    Clay_Hovered()
                        ? CLAY_BORDER_OUTSIDE({
                            .width = 3,
                            .color = colour_panel_menu_accent
                        })
                        : CLAY_BORDER_OUTSIDE({
                            .width = 1,
                            .color = colour_black
                        })
                ) {
                    CLAY_TEXT(
                        current_profile,
                        CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_TITLE,
                            .fontSize = 24,
                            .textColor = colour_text_label
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
     * */

    mainmenu_render_subheader(CLAY_STRING("Random Map"));

    CLAY(
        CLAY_ID("MainMenuPlayPanelRandomMapButtons"),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childGap = 12
        })
    ) {
        CLAY( CLAY_LAYOUT({ .sizing = { .width = CLAY_SIZING_PERCENT(0.6) } }) ) {}
        button_render(&playbutton_random, &buttonconfig_menu);
    }

    mainmenu_render_divline();
}

void mainmenu_render_aboutpanel(void)
{
    mainmenu_render_divline();

    mainmenu_render_subheader(CLAY_STRING("Credits"));

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
                .textColor = colour_text_body
            })
        );
    }

    mainmenu_render_divline();

    mainmenu_render_subheader(CLAY_STRING("Technical"));

    CLAY(
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .childGap = 12,
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
        })
    ) {
        CLAY_TEXT(
            CLAY_STRING("\
$ cat src/*.c src/hdr/*.h | \
sed /\\/\\*.*\\*\\//d | \
sed /\\/\\*/,/\\*\\//d | \
sed -n /[a-zA-Z]/p | wc -l\n\
> 1031\
            "),
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_TITLE,
                .fontSize = 18,
                .textColor = colour_text_body
            })
        );
        CLAY_TEXT(
            CLAY_STRING("\
     Code : C\n\
 Graphics : raylib  (github.com/raysan5/raylib)\n\
Interface : clay    (github.com/nicbarker/clay)\n\
            "),
            CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_TITLE,
                .fontSize = 18,
                .textColor = colour_text_body
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
            CLAY_RECTANGLE({ .color = colour_panel_menu, .cornerRadius = 24 }),
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
            switch (mainmenu_buttonarray.selected) {
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
        CLAY_RECTANGLE({ .color =  colour_haze }),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { .left = 24 },
            .layoutDirection = CLAY_LEFT_TO_RIGHT
        })
    )

    {
        CLAY(
            CLAY_ID("MainMenu"),
            CLAY_RECTANGLE({ .color = colour_panel_menu }),
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
            buttonarray_render(&mainmenu_buttonarray);
        }

        if (mainmenu_buttonarray.selected != BUTTON_NONE) {
            mainmenu_render_sidepanel();
        }
    }

    return Clay_EndLayout();
}


/*  IN-GAME     ***********************************************************************/



/*  IN-GAME     //  TOP BAR     *******************************************************/


void ingame_render_topbar(void)
{
    CLAY(
        CLAY_ID("InGameTopBar"),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(36) },
            .padding = { .left = 24, .right = 24, .top = 3, .bottom = 6 },
            .layoutDirection = CLAY_LEFT_TO_RIGHT
        }),
        CLAY_RECTANGLE({ .color = colour_panel_primary })
    )

    {
        CLAY(
            CLAY_LAYOUT({
                .sizing = { .width = 120, .height = CLAY_SIZING_GROW(0) }
            }),
            CLAY_RECTANGLE({ .color = colour_panel_primary_accent, .cornerRadius = 3 })
        ) { }
    }
}


/*  IN-GAME     //  BOTTOM BAR  *******************************************************/


void bottombar_hover(Clay_ElementId clay_id, Clay_PointerData mouse, intptr_t data);

#define NUM_BOTTOMBAR_BUTTONS 2
struct Button bottombar_buttons[NUM_BOTTOMBAR_BUTTONS] = {
    {
        .id = BOTTOMBAR_BUTTON_BUILD1,
        .label = CLAY_STRING("Build 1"),
        .on_hover = bottombar_hover,
    },
    {
        .id = BOTTOMBAR_BUTTON_BUILD2,
        .label = CLAY_STRING("Build 2"),
        .on_hover = bottombar_hover,
    }
};


struct ButtonArray bottombar_buttonarray = {
    .selected = BUTTON_NONE,
    .length = NUM_BOTTOMBAR_BUTTONS,
    .buttons = bottombar_buttons,
    .buttonconfig = &buttonconfig_primary,
    .layout = (Clay_LayoutConfig) {
        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
        .padding = { .top = 6, .bottom = 6, .left = 12, .right = 12 },
        .childGap = (12),
        .layoutDirection = CLAY_LEFT_TO_RIGHT
    }
};


void bottombar_hover(Clay_ElementId clay_id, Clay_PointerData mouse, intptr_t data)
{
    (void)clay_id;
    if (mouse.state != CLAY_POINTER_DATA_PRESSED_THIS_FRAME) return;
    buttonarray_click(&bottombar_buttonarray, (enum BUTTON_ID) data);
}


void ingame_render_bottombar()
{
    CLAY(
        CLAY_ID("InGameBotBar"),
        CLAY_LAYOUT({
            .sizing = {
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(45)
            },
            .padding = { .left = 24, .right = 24, .top = 3, .bottom = 6 },
            .layoutDirection = CLAY_LEFT_TO_RIGHT
        }),
        CLAY_RECTANGLE({ .color = colour_panel_primary })
    )

    {
        CLAY(
            CLAY_LAYOUT({
                .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0) }
            }),
            CLAY_RECTANGLE({ .color = colour_panel_primary_accent, .cornerRadius = 3 })
        ) {
            buttonarray_render(&bottombar_buttonarray);
        }
    }
}


Clay_RenderCommandArray interface_renderer_ingame(void)
{
    Clay_BeginLayout();

    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_LAYOUT({
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        })
    )

    {
        ingame_render_topbar();

        CLAY(
            CLAY_LAYOUT({
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_GROW(0)
                }
            })
        ) {}

        ingame_render_bottombar();
    }

    return Clay_EndLayout();
}


void interface_update_ingame(void)
{
    if (IsKeyPressed(KEY_ONE)) {
        buttonarray_click(&bottombar_buttonarray, BOTTOMBAR_BUTTON_BUILD1);
    } else if (IsKeyPressed(KEY_TWO)) {
        buttonarray_click(&bottombar_buttonarray, BOTTOMBAR_BUTTON_BUILD2);
    }

    switch (bottombar_buttonarray.selected) {
        case BOTTOMBAR_BUTTON_BUILD1:
            action_building_shadow(BUILDING_TEST_1);
            break;
        default:
            action_building_shadow(BUILDING_NONE);
            break;
    }
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


void interface_render(void)
{
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
    /* TODO threading ? */
    Clay_Raylib_Render(render_commands);
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
        case INFEX_SCREEN_GAME:
            interface_update_ingame();
            break;
        default:
            break;
    }
}


void interface_reset(void)
{
    buttonarray_click(&mainmenu_buttonarray, BUTTON_NONE);
    buttonarray_click(&bottombar_buttonarray, BUTTON_NONE);
}


#pragma GCC diagnostic pop
