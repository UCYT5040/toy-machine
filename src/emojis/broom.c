#include "emojis/broom.h"

#include "entities.h"
#include "world.h"

void emoji_broom_update(World *world, Entity *self, double dt) {
        (void)self;
        (void)dt;

        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *e = &world->entities[i];
                
                if (!e->active) {
                        continue;
                }
                
                if (e->definition == NULL) {
                        continue;
                }

                if (strcmp(e->definition->name, "coin") == 0) {
                        continue;
                }
                
                if (strcmp(e->definition->name, "purse") == 0) {
                        continue;
                }

                if (strcmp(e->definition->name, "money_bag") == 0) {
                        continue;
                }

                entity_remove(world, e);
        }
}