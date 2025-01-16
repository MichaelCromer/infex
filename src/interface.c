#include <stdlib.h>
#include <stdio.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#define CLAY_IMPLEMENTATION
#include "lib/clay/clay.h"
#include "lib/clay/renderers/raylib/clay_renderer_raylib.c"
#pragma GCC diagnostic pop

#include "hdr/infex.h"
#include "hdr/state.h"

#define INTERFACE_MAX_ELEMENTS 2048

uint32_t max_elements = INTERFACE_MAX_ELEMENTS;
uint64_t memory = 0;
Clay_Arena arena = { 0 };
Clay_Dimensions dimensions = { 0 };

Clay_RenderCommandArray (*interface_renderer)(void) = NULL;
Clay_RenderCommandArray render_commands = { 0 };

void clay_stderr(Clay_ErrorData error_data)
{
    fprintf(stderr, "%s\n", error_data.errorText.chars);
}

void interface_initialise(void)
{
    Clay_SetMaxElementCount(max_elements);

    memory = Clay_MinMemorySize();
    arena = Clay_CreateArenaWithCapacityAndMemory(memory, malloc(memory));
    dimensions = (Clay_Dimensions) { (float) screen_width(), (float) screen_height() };
    Clay_SetMeasureTextFunction(Raylib_MeasureText);

    Clay_Initialize(
        arena,
        dimensions,
        (Clay_ErrorHandler) { clay_stderr, 0 }
    );

}

Clay_RenderCommandArray interface_renderer_mainmenu()
{
    Clay_BeginLayout();

    CLAY(
        CLAY_ID("OuterContainer"),
        CLAY_LAYOUT({
            .sizing = {
                .width = CLAY_SIZING_FIXED(screen_width()),
                .height = CLAY_SIZING_FIXED(screen_height())
            },
            .padding = { 24, 24 },
            .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
        }),
        CLAY_RECTANGLE({ .color = { 200, 200, 200, 64 } })
    ) {
        CLAY(
            CLAY_ID("MainMenuContainer"),
            CLAY_LAYOUT({
                .sizing = {
                    .width = CLAY_SIZING_PERCENT(0.33),
                    .height = CLAY_SIZING_GROW(0),
                },
                .padding = { 24, 24 },
            }),
            CLAY_RECTANGLE({ .color = { 100, 100, 100, 255 } })
        ) {}
    }

    return Clay_EndLayout();
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
        }),
        CLAY_RECTANGLE({ .color = { 200, 200, 200, 0 } })
    ) { }

    return Clay_EndLayout();
}

void interface_update(float dt)
{
    (void)dt;

    switch (screen_curr()) {
        case INFEX_SCREEN_MAINMENU:
            interface_renderer = interface_renderer_mainmenu;
            break;
        default:
            interface_renderer = interface_renderer_fallback;
            break;
    }

    render_commands = interface_renderer();
}

void interface_render(void)
{
    Clay_Raylib_Render(render_commands);
}
