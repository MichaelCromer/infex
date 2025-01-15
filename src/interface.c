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

Clay_RenderCommandArray (*interface_renderer)(float dt) = NULL;
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

Clay_RenderCommandArray interface_renderer_fallback(float dt)
{
    (void)dt;

    Clay_BeginLayout();

    CLAY( 
        CLAY_ID("OuterContainer"),
        CLAY_LAYOUT({ 
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { 24, 24 }, 
            .childGap = 16
        }),
        CLAY_RECTANGLE({ .color = { 200, 200, 200, 255 } }) 
    ) { }

    return Clay_EndLayout();
}

void interface_update(float dt)
{
    switch (screen_curr()) {
        default:
            interface_renderer = interface_renderer_fallback;
    }

    render_commands = interface_renderer(dt);
}

void interface_render(void)
{
    Clay_Raylib_Render(render_commands);
}
