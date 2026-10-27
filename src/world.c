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
        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *entity = &world->entities[i];
                if (!entity->active) {
                        continue;
                }

                // TODO: Run update function for each entity, if it has one
        }

        if (world->dragged_entity_index != -1) {
                Entity *dragged_entity =
                    &world->entities[world->dragged_entity_index];

                dragged_entity->v.x =
                    (world->mouse_pos.x - dragged_entity->pos.x) * 20.0;
                dragged_entity->v.y =
                    (world->mouse_pos.y - dragged_entity->pos.y) * 20.0;
        }

        double substep_dt = dt / (double)PHYSICS_SUBSTEPS;
        for (int step = 0; step < PHYSICS_SUBSTEPS; step++) {
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *entity = &world->entities[i];

                        if (!entity->active)
                                continue;

                        entity->a = (Vec2){0.0, 0.0};

                        entity_physics_step(entity); // Computes `a`

                        entity->v.x += entity->a.x * substep_dt;
                        entity->v.y += entity->a.y * substep_dt;

                        entity->pos.x += entity->v.x * substep_dt;
                        entity->pos.y += entity->v.y * substep_dt;
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
        }
}
