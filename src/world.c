#include "world.h"

#define PHYSICS_SUBSTEPS 8

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
