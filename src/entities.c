#include "entities.h"
#include "emoji_registry.h"
#include "world.h"

void entity_create_default(Entity *entity, size_t definition_index, Vec2 pos) {
        entity->active = true;
        entity->definition = &EMOJI_DEFINITIONS[definition_index];
        entity->type = (EmojiType)definition_index;
        entity->pos = pos;
        entity->v = (Vec2){0.0, 0.0};
        entity->angle = 0.0;
        entity->scale = 1.0;
        entity->tags = 0;
        entity->timer = 0.0;
}

void entity_physics_step(Entity *entity, double *dt) {
        if (entity->definition->mass <= 0.0)
                return; // Object is immovable

        Vec2 a = (Vec2){0.0, 0.0};

        a.y = 9.81; // Homogenous gravitational field downwards

        entity->v.x += a.x * *dt;
        entity->v.y += a.y * *dt;

        entity->pos.x += entity->v.x * *dt;
        entity->pos.y += entity->v.y * *dt;
}