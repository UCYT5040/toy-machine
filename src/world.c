#include "world.h"
#include "app.h"
#include "collision.h"
#include "dev.h"
#include "emoji_registry.h"
#include "entities.h"
#include "popover.h"
#include "sidebar.h"
#include "tutorial.h"
#include "vault.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>

#define PHYSICS_SUBSTEPS 8
#define MAX_A 2500.0
#define DRAG_THRESHOLD 6.0f

void world_init(World *world, SDL_Renderer *renderer, SceneType scene_type) {
        *world = (World){0};

        if (scene_type == SCENE_TYPE_PLAY) {
                world->enable_box_and_space_background = true;
                world->enable_tutorial = true;
        } else {
                world->enable_box_and_space_background = false;
                world->enable_tutorial = false;
        }

        SDL_GetRendererOutputSize(renderer, &world->width, &world->height);

        world->gravity = (Vec2){0.0, 981.0};
        world->space_mode = false;

        world->pressed_entity_index =
            -1; // First element will be in a dragging state if not set to `-1`
        world->speed = 1.0; // If left unset, the game will be frozen (0.0)
        world->save_slot = 1;
        world->rain_channel = -1;

        vault_init(&world->vault, renderer);

        // Set all entities to inactive and collectable
        for (int i = 0; i < MAX_ENTITIES; i++) {
                world->entities[i].active = false;
                world->entities[i].collect = true;
        }

        if (world->enable_tutorial)
                tutorial_enter_step(world, TUTORIAL_STEP_START);

        for (int i = 0; i < MAX_RAINDROPS; i++) {
                Raindrop *drop = &world->raindrops[i];

                drop->pos.x = (float)(rand() % (world->width - 200) + 100);
                drop->pos.y = (float)(rand() % world->height);

                drop->speed = 450.0f + (float)(rand() % 300);

                drop->length = 12.0f + (drop->speed / 750.0f) * 10.0f;

                drop->alpha = 140.0f + (float)(rand() % 80);
        }
}

void world_update(App *app, World *world, double dt) {
        world->space_mode = false;
        bool rain_cloud_present = false;
        for (int i = 0; i < MAX_ENTITIES; i++) {
                if (world->entities[i].active) {
                        if (world->entities[i].tags & TAG_SPACE) {
                                world->space_mode = true;
                                break;
                        }
                        if (world->entities[i].type == EMOJI_RAIN_CLOUD &&
                            world->rain_intensity < 1.0) {
                                world->rain_intensity += dt * 0.05;
                                if (world->rain_intensity > 1.0) {
                                        world->rain_intensity = 1.0;
                                }
                                rain_cloud_present = true;
                        }
                }
        }

        if (!rain_cloud_present && world->rain_intensity > 0.0) {
                world->rain_intensity -=
                    dt * 0.1; // Faster decay that growth, since multiple
                              // clouds can cause more growth
                if (world->rain_intensity < 0.0) {
                        world->rain_intensity = 0.0;
                }
        }

        if (world->rain_intensity > 0.01f) {
                if (world->rain_channel == -1) {
                        world->rain_channel = Mix_PlayChannel(
                            -1, app->asset_manager.sfx_rain, -1);
                } else
                        Mix_Volume(
                            world->rain_channel,
                            (int)(world->rain_intensity * MIX_MAX_VOLUME));

                // Update raindrops
                for (int i = 0; i < MAX_RAINDROPS; i++) {
                        Raindrop *drop = &world->raindrops[i];
                        drop->pos.y += drop->speed * dt;
                        drop->pos.x -= drop->speed * 0.15f * dt;

                        if (drop->pos.y > world->height) {
                                drop->pos.y = -drop->length;
                                drop->pos.x = rand() % world->width;
                        }
                }
        } else if (world->rain_channel != -1) {
                Mix_HaltChannel(world->rain_channel);
                world->rain_channel = -1;
        }

        if (world->space_mode) {
                world->space_mode_transition += dt;
                if (world->space_mode_transition > 1.0)
                        world->space_mode_transition = 1.0;
        } else {
                world->space_mode_transition -= dt;
                if (world->space_mode_transition < 0.0)
                        world->space_mode_transition = 0.0;
        }

        double scaled_dt =
            dt * world->speed; // TODO: Is it necessary to do this in the parent
                               // loop? (i.e., is dt referenced anywhere else)

        Vec2 raw_mouse_accel =
            vec2_scale(vec2_sub(world->mouse_speed, world->last_mouse_speed),
                       1.0 / scaled_dt);
        world->last_mouse_speed = world->mouse_speed;

        // Smooth mouse acceleration to avoid jittery behavior
        world->mouse_accel = vec2_add(vec2_scale(world->mouse_accel, 0.85),
                                      vec2_scale(raw_mouse_accel, 0.15));

        double substep_dt = scaled_dt / (double)PHYSICS_SUBSTEPS;
        for (int step = 0; step < PHYSICS_SUBSTEPS; step++) {
                // Calculate `a` for all entities
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *entity = &world->entities[i];

                        if (!entity->active)
                                continue;

                        if (i == world->pressed_entity_index) {
                                if (world->is_dragging) {
                                        Vec2 relative_grab = vec2_rotate(
                                            world->press_local_offset,
                                            entity->angle);

                                        entity->pos = vec2_sub(world->mouse_pos,
                                                               relative_grab);

                                        entity->v = world->mouse_speed;

                                        if (entity->pinned) {
                                                entity->omega = 0.0;
                                                entity->torque = 0.0;
                                                continue;
                                        }

                                        double ax = fmax(
                                            -MAX_A,
                                            fmin(MAX_A, (world->mouse_accel.x *
                                                         -1.0)));
                                        double ay = fmax(
                                            -MAX_A,
                                            fmin(MAX_A, (world->mouse_accel.y *
                                                         -1.0)));
                                        Vec2 effective_accel = {ax, ay};

                                        double mass = entity->definition->mass;
                                        Vec2 grav_force =
                                            world->space_mode
                                                ? (Vec2){0.0, 0.0}
                                                : vec2_scale(world->gravity,
                                                             mass);
                                        Vec2 accel_force = vec2_scale(
                                            effective_accel, -0.25 * mass);
                                        Vec2 total_force =
                                            vec2_add(grav_force, accel_force);

                                        double swing_torque = vec2_cross(
                                            vec2_scale(relative_grab, -1.0),
                                            total_force);

                                        entity->torque = swing_torque;

                                        entity->omega += (entity->torque *
                                                          entity->inv_inertia) *
                                                         substep_dt;
                                        // Dampen velocity
                                        double speed_drag =
                                            1.8 + fabs(entity->omega) * 0.04;
                                        entity->omega *=
                                            (1.0 - speed_drag * substep_dt);
                                        entity->angle +=
                                            entity->omega * substep_dt;
                                        entity->torque =
                                            0.0; // Reset torque after applying
                                                 // it

                                        // Constrain dragged entity to bounds
                                        Vec2 contact_point;
                                        double penetration;

                                        if (get_lowest_point(
                                                entity, &contact_point,
                                                &penetration,
                                                (double)world->height)) {
                                                entity->pos.y -= penetration;
                                        }

                                        double radius =
                                            entity->definition->radius *
                                            entity->scale;
                                        if (entity->pos.x - radius < 0) {
                                                entity->pos.x = radius;
                                        }
                                        if (entity->pos.x + radius >
                                            world->width) {
                                                entity->pos.x =
                                                    world->width - radius;
                                        }
                                        if (entity->pos.y - radius < 0) {
                                                entity->pos.y = radius;
                                        }

                                        continue;
                                } else {
                                        float distance = vec2_distance(
                                            world->mouse_pos,
                                            world->press_start_pos);

                                        if (distance > DRAG_THRESHOLD) {
                                                world->is_dragging = true;
                                        }
                                }
                        }

                        entity_apply_forces(entity, world);
                }

                world_resolve_collisions(world);

                // Integrate acceleration and velocity
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *entity = &world->entities[i];

                        if (!entity->active ||
                            entity->definition->mass <= 0.0 || entity->pinned ||
                            i == world->pressed_entity_index)
                                continue;

                        entity_physics_integrate(entity, &substep_dt);

                        entity_constrain_to_bounds(entity, world);
                }
        }

        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *entity = &world->entities[i];

                if (!entity->active || !entity->definition->update) {
                        continue;
                }

                entity->definition->update(world, entity, scaled_dt);
        }

        if (world->enable_tutorial)
                tutorial_update(world, scaled_dt);
}

void world_render(App *app, World *world, SDL_Renderer *renderer) {
        if (!world->enable_box_and_space_background) {
                // Always render yard background for the title
                SDL_RenderCopy(renderer, app->asset_manager.bg_yard, NULL,
                               NULL);

        } else {
                SDL_RenderCopy(renderer, app->asset_manager.bg_box, NULL, NULL);

                if (world->space_mode_transition > 0.001) {
                        SDL_SetTextureBlendMode(app->asset_manager.bg_space,
                                                SDL_BLENDMODE_BLEND);
                        SDL_SetTextureAlphaMod(
                            app->asset_manager.bg_space,
                            (Uint8)(255.0 * world->space_mode_transition));
                        SDL_RenderCopy(renderer, app->asset_manager.bg_space,
                                       NULL, NULL);
                }
        }

        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *entity = &world->entities[i];
                if (entity == NULL || !entity->active) {
                        continue;
                }

                SDL_Texture *texture =
                    app->asset_manager.emojis[entity->definition->index];
                if (texture == NULL) {
                        continue;
                }

                double radius = entity->definition->radius * entity->scale;

                if (entity->tags & TAG_INVENTORY && entity->processing) {
                        // from 0 to 1
                        double tilt = fmod(entity->processing_timer, 1.0);
                        double x_offset =
                            radius * 0.1 *
                            sin(tilt * M_PI * 2.0); // Opposite for top/bottom

                        SDL_FPoint top_left = {entity->pos.x - radius +
                                                   x_offset,
                                               entity->pos.y - radius};
                        SDL_FPoint top_right = {entity->pos.x + radius +
                                                    x_offset,
                                                entity->pos.y - radius};
                        SDL_FPoint bottom_left = {entity->pos.x - radius -
                                                      x_offset,
                                                  entity->pos.y + radius};
                        SDL_FPoint bottom_right = {entity->pos.x + radius -
                                                       x_offset,
                                                   entity->pos.y + radius};

                        SDL_Color color = {255, 255, 255,
                                           255}; // Retains texture

                        SDL_Vertex vertices[4] = {
                            {top_left, color, {0.0, 0.0}},
                            {top_right, color, {1.0, 0.0}},
                            {bottom_right, color, {1.0, 1.0}},
                            {bottom_left, color, {0.0, 1.0}}};
                        int indices[6] = {0, 1, 2, 2, 3, 0};

                        SDL_RenderGeometry(renderer, texture, vertices, 4,
                                           indices, 6);
                } else {

                        SDL_FRect dest_rect = {entity->pos.x - radius,
                                               entity->pos.y - radius,
                                               radius * 2.0, radius * 2.0};

                        SDL_RenderCopyExF(renderer, texture, NULL, &dest_rect,
                                          entity->angle * (180.0 / M_PI), NULL,
                                          SDL_FLIP_NONE);
                }

                if (entity->tags & TAG_INVENTORY &&
                    entity->processing_input_count > 0) {
                        switch (entity->definition->processing_display) {
                        case EMOJI_PROCESSING_DISPLAY_SUM:
                        default: {
                                float icon_size = radius * 0.7;
                                float spacing = radius * 0.1;

                                float content_width =
                                    entity->processing_input_count * icon_size +
                                    (entity->processing_input_count - 1) *
                                        spacing;
                                float content_height = icon_size;

                                Vec2 popover_size = {content_width,
                                                     content_height};
                                Vec2 tip = {entity->pos.x,
                                            entity->pos.y - radius * 1.5};

                                popover_draw_background(renderer, tip,
                                                        popover_size,
                                                        POPOVER_POSITION_TOP);
                                SDL_FRect popover_bounds = popover_get_bounds(
                                    tip, popover_size, POPOVER_POSITION_TOP);

                                float start_x = popover_bounds.x;
                                float icon_y = popover_bounds.y;

                                for (int j = 0;
                                     j < entity->processing_input_count; j++) {
                                        int input_index =
                                            entity->processing_inputs[j];
                                        if (input_index >= 0 &&
                                            input_index < MAX_ENTITIES) {
                                                Entity *input_entity =
                                                    &world->entities
                                                         [input_index];

                                                SDL_Texture *input_texture =
                                                    app->asset_manager
                                                        .emojis[input_entity
                                                                    ->definition
                                                                    ->index];
                                                if (input_texture == NULL) {
                                                        continue;
                                                }

                                                SDL_FRect input_dest_rect = {
                                                    start_x + j * (icon_size +
                                                                   spacing),
                                                    icon_y, icon_size,
                                                    icon_size};

                                                SDL_RenderCopyF(
                                                    renderer, input_texture,
                                                    NULL, &input_dest_rect);
                                        }
                                }
                                break;
                        }
                        }
                }

                if (world->dev_show_velocity_vectors)
                        render_velocity_vector(renderer, entity);

                if (world->dev_show_hitboxes)
                        render_hitbox(renderer, entity);
        }

        int visible_drops = (int)(MAX_RAINDROPS * world->rain_intensity);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        for (int i = 0; i < visible_drops; i++) {
                Raindrop *drop = &world->raindrops[i];
                Uint8 a = (Uint8)(drop->alpha * world->rain_intensity);
                SDL_SetRenderDrawColor(renderer, 180, 205, 230, a);
                SDL_RenderDrawLineF(renderer, drop->pos.x, drop->pos.y,
                                    drop->pos.x - 3.0f,
                                    drop->pos.y + drop->length);
        }

        if (world->rain_intensity > 0.01f) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                Uint8 alpha = (Uint8)(110.0f * world->rain_intensity);
                SDL_SetRenderDrawColor(renderer, 20, 30, 45, alpha);
                SDL_RenderFillRect(renderer, NULL);
        }
}

void world_handle_mouse_down(World *world) {
        int entity_index = world_get_entity_at(world, world->mouse_pos);

        if (entity_index != -1) {
                world->pressed_entity_index = entity_index;

                Entity *pressed = &world->entities[entity_index];

                Vec2 d = vec2_sub(world->mouse_pos, pressed->pos);
                world->press_local_offset = vec2_rotate(d, -pressed->angle);
                world->press_start_pos = world->mouse_pos;
        }
}

void world_handle_mouse_up(App *app, World *world) {
        uint32_t time = SDL_GetTicks();
        double mouse_dt =
            (double)time / 1000.0 - (double)world->last_mouse_move / 1000.0;

        if (world->pressed_entity_index != -1) {
                if (world->is_dragging) {
                        Entity *dragged =
                            &world->entities[world->pressed_entity_index];

                        if (world->sidebar_shown &&
                            pos_in_sidebar(app, &world->mouse_pos)) {
                                entity_remove(world, dragged);
                        } else {
                                if (!dragged->pinned && mouse_dt < 0.1)
                                        dragged->v = world->mouse_speed;
                                else
                                        dragged->v = (Vec2){0.0, 0.0};
                        }

                        world->is_dragging = false;
                } else {
                        Entity *pressed =
                            &world->entities[world->pressed_entity_index];

                        if (pressed->definition->click) {
                                pressed->definition->click(app, world, pressed);
                        }
                }
        }

        world->pressed_entity_index = -1;
}

int world_get_entity_at(World *world, Vec2 pos) {
        int nearest = -1;
        double nearest_distance_sq = 0.0;

        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *entity = &world->entities[i];
                if (!entity->active) {
                        continue;
                }

                double distance_sq = vec2_distance_squared(pos, entity->pos);

                double radius = entity->definition->radius * entity->scale;

                if (distance_sq <= radius * radius) {
                        if (nearest == -1 ||
                            distance_sq < nearest_distance_sq) {
                                nearest = i;
                                nearest_distance_sq = distance_sq;
                        }
                }
        }

        return nearest;
}
