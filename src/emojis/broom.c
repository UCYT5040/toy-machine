#include "emojis/broom.h"

#include "entities.h"
#include "world.h"

void emoji_broom_update(World *world, Entity *self, double dt) {
        (void)self;
        (void)dt;

        for (int i = 0; i < MAX_ENTITIES; i++) {
                if (world->entities[i].active) {
                        entity_remove(world, &world->entities[i]);
                }
        }
}