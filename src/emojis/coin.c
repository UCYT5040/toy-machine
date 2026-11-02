#include "emojis/coin.h"

#include "vault.h"

#define COIN_LIFETIME                                                          \
        5.0 /* How long coin remains physical before being deposited and       \
               removed from screen */

void emoji_coin_update(World *world, Entity *self, double dt) {
        if (self->despawn_timer < 0.0) {
                self->despawn_timer = COIN_LIFETIME;
        }

        self->despawn_timer -= dt;

        if (self->despawn_timer > 0.0) {
                return;
        }

        /* Deposit coin into vault after despawning */
        vault_add(&world->vault, 1);

        if (world->dragged_entity_index == self->index) {
                world->dragged_entity_index = -1;
        }

        self->active = false;
        self->collect = true;
}