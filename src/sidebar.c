#include "sidebar.h"
#include "emoji_registry.h"
#include "entities.h"
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

        // Only draws the title if not scrolled
        if (world->sidebar_scroll_px <= 1.0)
                font_draw(font, renderer, "Your Items",
                          sidebar_x + (float)sidebar_width / 2 -
                              (float)title_width / 2.0,
                          10.0, font_color);

        float item_y = 40.0 - world->sidebar_scroll_px;
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

                if (item_y + 35.0 + emoji_h > 0.0 &&
                    item_y - (35.0 + emoji_h) < renderer_h) {
                        SDL_FRect emoji_rect = {(float)sidebar_x + 20.0, item_y,
                                                emoji_w, emoji_h};

                        SDL_RenderCopyF(renderer, texture, NULL, &emoji_rect);

                        font_draw(font, renderer, EMOJI_DEFINITIONS[i].name,
                                  sidebar_x + 20.0, item_y + emoji_h + 5.0,
                                  font_color);
                }

                item_y += 35.0 + emoji_h;
        }
}

void sidebar_handle_mouse_down(World *world) { (void)world; }

void sidebar_handle_scroll(SDL_Renderer *renderer, World *world, float y) {
        int renderer_w, renderer_h;
        SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

        float max_scroll = 0;
        for (int i = 0; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];

                if (definition->unlocked && world->textures[i])
                        max_scroll +=
                            35.0 +
                            (definition->colliders->type == COLLIDER_BOX
                                 ? definition->colliders->box.width
                                 : definition->colliders->circle.radius * 2.0);
        }

        float limit = fmaxf(max_scroll - renderer_h + 40.0f, 0.0f);
        world->sidebar_scroll_px = fminf(
            fmaxf(world->sidebar_scroll_px + y * SCROLL_FACTOR, 0.0f), limit);
}

bool pos_in_sidebar(SDL_Renderer *renderer, float x) {
        int renderer_w, renderer_h;

        SDL_GetRendererOutputSize(renderer, &renderer_w, &renderer_h);

        int sidebar_x = (int)fmax(renderer_w - SIDEBAR_WIDTH, 0.0);

        return x >= sidebar_x;
}