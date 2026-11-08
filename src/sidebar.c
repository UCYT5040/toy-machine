#include "sidebar.h"
#include "app.h"
#include "emoji_registry.h"
#include "entities.h"
#include "font.h"
#include "types.h"
#include "vec2.h"

#include <SDL2/SDL.h>
#include <math.h>

static float get_emoji_height(EmojiDefinition *definition) {
        switch (definition->colliders->type) {
        case COLLIDER_BOX:
                return definition->colliders->box.height;
        case COLLIDER_CIRCLE:
                return definition->colliders->circle.radius * 2.0;
        default:
                return definition->radius * 2.0;
        }
}

static float get_emoji_width(EmojiDefinition *definition) {
        switch (definition->colliders->type) {
        case COLLIDER_BOX:
                return definition->colliders->box.width;
        case COLLIDER_CIRCLE:
                return definition->colliders->circle.radius * 2.0;
        default:
                return definition->radius * 2.0;
        }
}

void draw_sidebar(App *app, World *world, FontAtlas *font) {
        int renderer_w, renderer_h;
        SDL_GetRendererOutputSize(app->renderer, &renderer_w, &renderer_h);

        int sidebar_x = (int)fmax(renderer_w - SIDEBAR_WIDTH, 0.0);
        int sidebar_width = renderer_w - sidebar_x;
        const SDL_Rect sidebar_rect = {
            .x = sidebar_x, .y = 0, .w = sidebar_width, .h = renderer_h};

        SDL_SetRenderDrawColor(app->renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(app->renderer, &sidebar_rect);

        const char title[] = "Your Items";
        int title_width = font_measure_text(font, title);

        SDL_Color font_color = {.r = 255, .g = 255, .b = 255, .a = 255};

        // Only draws the title if not scrolled
        if (world->sidebar_scroll_px <= 1.0)
                font_draw(font, app->renderer, "Your Items",
                          sidebar_x + (float)sidebar_width / 2 -
                              (float)title_width / 2.0,
                          10.0, font_color);

        float item_y = 40.0 - world->sidebar_scroll_px;
        for (int i = 1; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];
                SDL_Texture *texture = app->asset_manager.emojis[i];

                if (!definition->unlocked || !texture)
                        continue;

                float emoji_w = get_emoji_width(definition);
                float emoji_h = get_emoji_height(definition);

                if (item_y + 35.0 + emoji_h > 0.0 &&
                    item_y - (35.0 + emoji_h) < renderer_h) {
                        SDL_FRect emoji_rect = {(float)sidebar_x + 20.0, item_y,
                                                emoji_w, emoji_h};

                        SDL_RenderCopyF(app->renderer, texture, NULL,
                                        &emoji_rect);

                        font_draw(font, app->renderer,
                                  EMOJI_DEFINITIONS[i].name, sidebar_x + 20.0,
                                  item_y + emoji_h + 5.0, font_color);
                }

                item_y += 35.0 + emoji_h;
        }
}

void sidebar_handle_mouse_down(App *app, World *world) {
        int renderer_w, renderer_h;
        SDL_GetRendererOutputSize(app->renderer, &renderer_w, &renderer_h);

        int sidebar_x = (int)fmax(renderer_w - SIDEBAR_WIDTH, 0.0);

        float item_y = 40.0 - world->sidebar_scroll_px;
        Vec2 spawn_pos;
        EmojiType emoji_type;
        bool spawn_found = false;
        for (int i = 1; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];
                SDL_Texture *texture = app->asset_manager.emojis[i];

                if (!definition->unlocked || !texture)
                        continue;

                float x = (float)sidebar_x + 20.0;
                float y = item_y;

                switch (definition->colliders->type) {
                case COLLIDER_BOX: {
                        float w = definition->colliders->box.width;
                        float h = definition->colliders->box.height;

                        if (world->mouse_pos.x >= x &&
                            world->mouse_pos.x <= x + w &&
                            world->mouse_pos.y >= y &&
                            world->mouse_pos.y <= y + h) {
                                spawn_pos = (Vec2){x + w / 2, y + h / 2};
                                emoji_type = i;
                                spawn_found = true;
                        }

                        break;
                }
                case COLLIDER_CIRCLE: {
                        Vec2 center =
                            (Vec2){x + definition->colliders->circle.radius,
                                   y + definition->colliders->circle.radius};

                        if (vec2_distance(world->mouse_pos, center) <=
                            definition->colliders->circle.radius) {
                                spawn_pos = center;
                                emoji_type = i;
                                spawn_found = true;
                        }
                }
                default: {
                        /*
                        This treats it like a circle using `definition->radius`
                        for any calculations
                        */

                        Vec2 center = (Vec2){x + definition->radius,
                                             y + definition->radius};

                        if (vec2_distance(world->mouse_pos, center) <=
                            definition->radius) {
                                spawn_pos = center;
                                emoji_type = i;
                                spawn_found = true;
                        }
                }
                }

                if (spawn_found)
                        break;

                item_y += 35.0 + get_emoji_height(definition);
        }

        if (spawn_found) {
                world->sidebar_shown = false;

                Entity *new_entity = entity_spawn(world, emoji_type, spawn_pos);
                if (!new_entity)
                        return;

                world->pressed_entity_index = new_entity->index;
                world->press_local_offset =
                    vec2_sub(world->mouse_pos, spawn_pos);
        }
}

void sidebar_handle_scroll(App *app, World *world, float y) {
        int renderer_w, renderer_h;
        SDL_GetRendererOutputSize(app->renderer, &renderer_w, &renderer_h);

        float max_scroll = 0;
        for (int i = 1; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];

                if (definition->unlocked && app->asset_manager.emojis[i])
                        max_scroll += 35.0 + get_emoji_height(definition);
        }

        float limit = fmaxf(max_scroll - renderer_h + 40.0f, 0.0f);
        world->sidebar_scroll_px = fminf(
            fmaxf(world->sidebar_scroll_px + y * SCROLL_FACTOR, 0.0f), limit);
}

bool pos_in_sidebar(App *app, Vec2 *pos) {
        int renderer_w, renderer_h;

        SDL_GetRendererOutputSize(app->renderer, &renderer_w, &renderer_h);

        int sidebar_x = (int)fmax(renderer_w - SIDEBAR_WIDTH, 0.0);

        return pos->x >= sidebar_x;
}