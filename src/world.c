#include "world.h"
#include "collision.h"
#include "dev.h"
#include "emoji_registry.h"
#include "entities.h"
#include "vault.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>

#define PHYSICS_SUBSTEPS 8
#define MAX_A 2500.0

void world_init(World *world, SDL_Renderer *renderer) {
        *world = (World){0};

        SDL_GetRendererOutputSize(renderer, &world->width, &world->height);

        world->gravity = (Vec2){0.0, 981.0};
        world->space_mode = false;

        world->dragged_entity_index =
            -1; // First element will be in a dragging state if not set to `-1`
        world->speed = 1.0; // If left unset, the game will be frozen (0.0)

        vault_init(&world->vault, renderer);

        for (int i = 1; i < EMOJI_COUNT; i++) {
                EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];

                definition->index = i;

                world->textures[i] = NULL; // Initialize texture to NULL

                if (definition->svg_path) {
                        SDL_Surface *surface = IMG_Load(definition->svg_path);

                        if (surface) {
                                world->textures[i] =
                                    SDL_CreateTextureFromSurface(renderer,
                                                                 surface);
                        }

                        SDL_FreeSurface(surface);
                }
        }

        SDL_Surface *bg_box_surface = IMG_Load("assets/box.jpg");
        SDL_Surface *bg_space_surface = IMG_Load("assets/space.jpg");

        if (!bg_box_surface || !bg_space_surface) {
                return;
        }
        world->bg_box = SDL_CreateTextureFromSurface(renderer, bg_box_surface);
        world->bg_space =
            SDL_CreateTextureFromSurface(renderer, bg_space_surface);
        SDL_FreeSurface(bg_box_surface);
        SDL_FreeSurface(bg_space_surface);

        SDL_SetTextureColorMod(world->bg_box, 190, 190,
                               190); // Dim the box background

        // Set all entities to inactive and collectable
        for (int i = 0; i < MAX_ENTITIES; i++) {
                world->entities[i].active = false;
                world->entities[i].collect = true;
        }
}

void world_update(World *world, double dt) {
        world->space_mode = false;
        for (int i = 0; i < MAX_ENTITIES; i++) {
                if (world->entities[i].active &&
                    (world->entities[i].tags & TAG_SPACE)) {
                        world->space_mode = true;
                        break;
                }
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

                        if (i == world->dragged_entity_index) {
                                Vec2 relative_grab = vec2_rotate(
                                    world->drag_local_offset, entity->angle);

                                entity->pos =
                                    vec2_sub(world->mouse_pos, relative_grab);

                                entity->v = world->mouse_speed;

                                if (entity->pinned) {
                                        entity->omega = 0.0;
                                        entity->torque = 0.0;
                                        continue;
                                }

                                double ax = fmax(
                                    -MAX_A,
                                    fmin(MAX_A, (world->mouse_accel.x * -1.0)));
                                double ay = fmax(
                                    -MAX_A,
                                    fmin(MAX_A, (world->mouse_accel.y * -1.0)));
                                Vec2 effective_accel = {ax, ay};

                                double mass = entity->definition->mass;
                                Vec2 grav_force =
                                    world->space_mode
                                        ? (Vec2){0.0, 0.0}
                                        : vec2_scale(world->gravity, mass);
                                Vec2 accel_force =
                                    vec2_scale(effective_accel, -0.25 * mass);
                                Vec2 total_force =
                                    vec2_add(grav_force, accel_force);

                                double swing_torque =
                                    vec2_cross(vec2_scale(relative_grab, -1.0),
                                               total_force);

                                entity->torque = swing_torque;

                                entity->omega +=
                                    (entity->torque * entity->inv_inertia) *
                                    substep_dt;
                                // Dampen velocity
                                double speed_drag =
                                    1.8 + fabs(entity->omega) * 0.04;
                                entity->omega *=
                                    (1.0 - speed_drag * substep_dt);
                                entity->angle += entity->omega * substep_dt;
                                entity->torque =
                                    0.0; // Reset torque after applying it

                                // Constrain dragged entity to bounds
                                Vec2 contact_point;
                                double penetration;

                                if (get_lowest_point(entity, &contact_point,
                                                     &penetration,
                                                     (double)world->height)) {
                                        entity->pos.y -= penetration;
                                }

                                double radius =
                                    entity->definition->radius * entity->scale;
                                if (entity->pos.x - radius < 0) {
                                        entity->pos.x = radius;
                                }
                                if (entity->pos.x + radius > world->width) {
                                        entity->pos.x = world->width - radius;
                                }
                                if (entity->pos.y - radius < 0) {
                                        entity->pos.y = radius;
                                }

                                continue;
                        }

                        entity_apply_forces(entity, world);
                }

                world_resolve_collisions(world);

                // Integrate acceleration and velocity
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *entity = &world->entities[i];

                        if (!entity->active ||
                            entity->definition->mass <= 0.0 || entity->pinned ||
                            i == world->dragged_entity_index)
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
}

void world_render(World *world, SDL_Renderer *renderer) {
        SDL_RenderCopy(renderer, world->bg_box, NULL, NULL);

        if (world->space_mode_transition > 0.001) {
                SDL_SetTextureBlendMode(world->bg_space, SDL_BLENDMODE_BLEND);
                SDL_SetTextureAlphaMod(
                    world->bg_space,
                    (Uint8)(255.0 * world->space_mode_transition));
                SDL_RenderCopy(renderer, world->bg_space, NULL, NULL);
        }

        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *entity = &world->entities[i];
                if (entity == NULL || !entity->active) {
                        continue;
                }

                SDL_Texture *texture =
                    world->textures[entity->definition->index];
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
                        default:
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
                                                    world->textures
                                                        [input_entity
                                                             ->definition
                                                             ->index];
                                                if (input_texture == NULL) {
                                                        continue;
                                                }

                                                double input_radius =
                                                    0.5 * radius;

                                                SDL_FRect input_dest_rect = {
                                                    entity->pos.x -
                                                        input_radius * 1.5 +
                                                        j * input_radius * 3.0,
                                                    entity->pos.y -
                                                        input_radius * 1.5,
                                                    input_radius * 2.0,
                                                    input_radius * 2.0};

                                                SDL_RenderCopyF(
                                                    renderer, input_texture,
                                                    NULL, &input_dest_rect);
                                        }
                                }
                                break;
                        }
                }

                if (world->dev_show_velocity_vectors)
                        render_velocity_vector(renderer, entity);

                if (world->dev_show_hitboxes)
                        render_hitbox(renderer, entity);
        }
}

void world_handle_mouse_down(World *world) {
        int entity_index = world_get_entity_at(world, world->mouse_pos);

        if (entity_index != -1) {
                world->dragged_entity_index = entity_index;

                Entity *dragged = &world->entities[entity_index];

                Vec2 d = vec2_sub(world->mouse_pos, dragged->pos);
                world->drag_local_offset = vec2_rotate(d, -dragged->angle);
        }
}

void world_handle_mouse_up(World *world) {
        uint32_t time = SDL_GetTicks();
        double mouse_dt =
            (double)time / 1000.0 - (double)world->last_mouse_move / 1000.0;

        if (world->dragged_entity_index != -1) {
                Entity *dragged = &world->entities[world->dragged_entity_index];
                if (!dragged->pinned && mouse_dt < 0.1)
                        dragged->v = world->mouse_speed;
                else
                        dragged->v = (Vec2){0.0, 0.0};
        }

        world->dragged_entity_index = -1;
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
