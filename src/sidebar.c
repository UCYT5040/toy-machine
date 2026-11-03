#include "sidebar.h"
#include "emoji_registry.h"
#include "font.h"
#include "types.h"

#include <SDL2/SDL.h>
#include <SDL_render.h>
#include <math.h>

void draw_sidebar(SDL_Renderer *renderer, World *world, FontAtlas *font) {
        int renderer_w, renderer_h;
        SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

        int sidebar_x = (int)fmax(renderer_w - SIDEBAR_WIDTH, 0.0);
        int sidebar_width = renderer_w - sidebar_x;
        const SDL_Rect sidebar_rect = {
            .x = sidebar_x, .y = 0, .w = sidebar_width, .h = renderer_h};

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &sidebar_rect);

        // White
        SDL_Color font_color = {.r = 255, .g = 255, .b = 255, .a = 255};

        font_draw(font, renderer, "Your Items", sidebar_x + 10.0, 10.0,
                  font_color);

        int item_y = 0;
        for (int i = 0; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];

                if (definition->unlocked == true) {
                        font_draw(font, renderer, EMOJI_DEFINITIONS[i].name,
                                  sidebar_x + 10.0, 40 + item_y * 20,
                                  font_color);
                        item_y++;
                }
        }
}