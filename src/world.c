#include "world.h"
#include "emoji_registry.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define PHYSICS_SUBSTEPS 8

void world_init(World *world, SDL_Renderer *renderer) {
  *world = (World){0};

  for (int i = 1; i < EMOJI_COUNT; i++) {
    EmojiDefinition *definition = &EMOJI_DEFINITIONS[i];

    definition->index = i;

    world->textures[i] = NULL; // Initialize texture to NULL

    if (definition->svg_path) {
      SDL_Surface *surface = IMG_Load(definition->svg_path);

      if (surface) {
        world->textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
      }
    }
  }
}

void world_update(World *world, float dt) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    Entity *entity = &world->entities[i];
    if (!entity->active) {
      continue;
    }

    // TODO: Run update function for each entity, if it has one
  }

  if (world->dragged_entity_index != -1) {
    Entity *dragged_entity = &world->entities[world->dragged_entity_index];

    dragged_entity->velocity.x =
        (world->mouse_pos.x - dragged_entity->position.x) * 20.0f;
    dragged_entity->velocity.y =
        (world->mouse_pos.y - dragged_entity->position.y) * 20.0f;
  }

  float substep_dt = dt / (float)PHYSICS_SUBSTEPS;
  for (int step = 0; step < PHYSICS_SUBSTEPS; step++) {
    // TODO: Pass to a physics_step function
  }

  for (int i = 0; i < MAX_ENTITIES; i++) {
    world->entities[i].force = (Vec2){0.0f, 0.0f};
  }
}

void world_render(World *world, SDL_Renderer *renderer) {
  for (int i = 0; i < MAX_ENTITIES; i++) {
    Entity *entity = &world->entities[i];
    if (entity == NULL || !entity->active) {
      continue;
    }

    SDL_Texture *texture = world->textures[entity->definition->index];
    if (texture == NULL) {
      continue;
    }

    SDL_FRect dest_rect = {
        entity->position.x - entity->definition->radius *
                                 entity->definition->scale * entity->scale,
        entity->position.y - entity->definition->radius *
                                 entity->definition->scale * entity->scale,
        entity->definition->radius * 2.0f * entity->definition->scale *
            entity->scale,
        entity->definition->radius * 2.0f * entity->definition->scale *
            entity->scale};

    SDL_RenderCopyF(renderer, texture, NULL, &dest_rect);
  }
}
