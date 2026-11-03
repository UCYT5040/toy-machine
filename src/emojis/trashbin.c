#include "emojis/trashbin.h"

#include "emoji_registry.h"
#include "entities.h"
#include "world.h"

void emoji_trashbin_collide(World *world, Entity *self, Entity *other) {
        if (!self->active || !other->active || self == other) {
                return;
        }

        if (other->type == EMOJI_TRASHBIN) {
                /* So that alot of trashbins can clear everything including
                 * themselves */
                entity_remove(world, self);
                entity_remove(world, other);
                return;
        }

        entity_remove(world, other);
}