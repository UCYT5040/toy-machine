#include "sidebar.h"
#include "emoji_registry.h"
#include "font.h"
#include "types.h"

#include <SDL2/SDL.h>
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

        const char title[] = "Your Items";
        int title_width = font_measure_text(font, title);

        SDL_Color font_color = {.r = 255, .g = 255, .b = 255, .a = 255};

        font_draw(font, renderer, "Your Items",
                  sidebar_x + (float)sidebar_width / 2 -
                      (float)title_width / 2.0,
                  10.0, font_color);

        float item_y = 40.0;
        for (int i = 0; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];
                SDL_Texture *texture = world->textures[i];

                if (!definition->unlocked || !texture)
                        continue;

                float emoji_w =
                    definition->colliders->type == COLLIDER_BOX
                        ? definition->colliders->box.width
                        : definition->colliders->circle.radius * 2.0;

                float emoji_h =
                    definition->colliders->type == COLLIDER_BOX
                        ? definition->colliders->box.width
                        : definition->colliders->circle.radius * 2.0;

                SDL_FRect emoji_rect = {(float)sidebar_x + 20.0, item_y,
                                        emoji_w, emoji_h};

                SDL_RenderCopyF(renderer, texture, NULL, &emoji_rect);

                item_y += emoji_h + 5.0;

                font_draw(font, renderer, EMOJI_DEFINITIONS[i].name,
                          sidebar_x + 20.0, item_y, font_color);

                item_y += 30.0;
        }
}

void sidebar_handle_mouse_down(World *world) { (void)world; }

bool pos_in_sidebar(SDL_Renderer *renderer, float x) {
        int renderer_w, renderer_h;

        SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

        int sidebar_x = (int)fmax(renderer_w - SIDEBAR_WIDTH, 0.0);

        return x >= sidebar_x;
}