#include "world.h"
#include "emoji_registry.h"
#include "entities.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define PHYSICS_SUBSTEPS 8

void world_init(World *world, SDL_Renderer *renderer) {
        *world = (World){0};

        world->dragged_entity_index =
            -1; // First element will be in a dragging state if not set to `-1`
        world->speed = 1.0; // If left unset, the game will be frozen (0.0)

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
                }
        }
}

void world_update(World *world, double dt) {
        double scaled_dt =
            dt * world->speed; // TODO: Is it necessary to do this in the parent
                               // loop? (i.e., is dt referenced anywhere else)

        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *entity = &world->entities[i];
                if (!entity->active) {
                        continue;
                }

                // TODO: Run update function for each entity, if it has one
        }

        double substep_dt = scaled_dt / (double)PHYSICS_SUBSTEPS;
        for (int step = 0; step < PHYSICS_SUBSTEPS; step++) {
                // Calculate `a` for all entities
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *entity = &world->entities[i];

                        if (!entity->active)
                                continue;

                        entity->a = (Vec2){0.0, 0.0};

                        if (i == world->dragged_entity_index) {
                                entity->pos = world->mouse_pos;
                                continue;
                        }

                        entity_physics_step(entity, world);
                }

                // Integrate acceleration and velocity
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *entity = &world->entities[i];

                        if (!entity->active ||
                            entity->definition->mass <= 0.0 ||
                            i == world->dragged_entity_index)
                                continue;

                        entity_physics_integrate(entity, &substep_dt);
                }
        }
}

void world_render(World *world, SDL_Renderer *renderer) {
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

                SDL_FRect dest_rect = {entity->pos.x - radius,
                                       entity->pos.y - radius, radius * 2.0,
                                       radius * 2.0};

                SDL_RenderCopyF(renderer, texture, NULL, &dest_rect);

                // TODO: Render hitboxes
        }
}

void world_handle_mouse_down(World *world) {
        int entity_index = world_get_entity_at(world, world->mouse_pos);

        if (entity_index != -1) {
                world->dragged_entity_index = entity_index;
        }
}

void world_handle_mouse_up(World *world) { world->dragged_entity_index = -1; }

int world_get_entity_at(World *world, Vec2 pos) {
        int nearest = -1;
        double nearest_distance_sq = 0.0;

        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *entity = &world->entities[i];
                if (!entity->active) {
                        continue;
                }

                double dx = pos.x - entity->pos.x;
                double dy = pos.y - entity->pos.y;
                double distance_sq = dx * dx + dy * dy;

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
