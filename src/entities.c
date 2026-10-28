#include "entities.h"
#include "emoji_registry.h"
#include "world.h"

void entity_create_default(Entity *entity, int definition_index, Vec2 pos,
                           int entity_index) {
        entity->active = true;
        entity->definition = &EMOJI_DEFINITIONS[definition_index];
        entity->type = (EmojiType)definition_index;
        entity->pos = pos;
        entity->v = (Vec2){0.0, 0.0};
        entity->angle = 0.0;
        entity->scale = 1.0;
        entity->tags = entity->definition->default_tags;
        entity->timer = 0.0;
        entity->index = entity_index;
        entity->a = (Vec2){0.0, 0.0};
}

void entity_physics_step(Entity *entity) {
        if (entity->definition->mass <= 0.0)
                return; // Object is immovable

        entity->a.y = 9.81; // Homogenous gravitational field downwards
}

void entity_physics_integrate(Entity *entity, double *dt) {
        entity->v.x += entity->a.x * *dt;
        entity->v.y += entity->a.y * *dt;

        entity->pos.x += entity->v.x * *dt;
        entity->pos.y += entity->v.y * *dt;
}