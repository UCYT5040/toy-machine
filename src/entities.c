#include "entities.h"
#include "emoji_registry.h"
#include "world.h"

void entity_create_default(Entity *entity, size_t definition_index, Vec2 pos) {
        entity->active = true;
        entity->definition = &EMOJI_DEFINITIONS[definition_index];
        entity->type = (EmojiType)definition_index;
        entity->pos = pos;
        entity->v = (Vec2){0.0, 0.0};
        entity->a = (Vec2){0.0, 0.0};
        entity->angle = 0.0;
}

void entity_physics_step(Entity *entity) {
        switch (entity->type) {
        case EMOJI_SOCCER_BALL:
                entity->a.y = 9.81; /* Temporary */
                break;
        default:
                break;
        };
}