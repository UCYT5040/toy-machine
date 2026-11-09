#include "emojis/bank.h"

#include <stdbool.h>

#include "emoji_registry.h"
#include "entities.h"

#define ATM_SHAKE_SPEED                                                        \
        400.0 /* Minimum speed for the emoji to be moving for coin drop to     \
                 trigger */
#define ATM_COIN_DELAY                                                         \
        3.0 /* Minimum distance for the emoji to be moving for coin drop to    \
                  trigger */
#define ATM_COIN_DOWN_SPEED 220.0
#define ATM_COIN_SIDE_SPEED 80.0

void emoji_atm_update(World *world, Entity *self, double dt) {
        if (dt <= 0.0) {
                return;
        }

        self->shake_distance += dt;

        if (self->shake_distance < ATM_COIN_DELAY) {
                return;
        }

        double bank_radius = self->definition->radius * self->scale;
        double coin_radius = EMOJI_DEFINITIONS[EMOJI_COIN].radius;
        Vec2 spawn_pos =
            vec2_add(self->pos, (Vec2){0.0, bank_radius + coin_radius + 2.0});

        Entity *coin = entity_spawn(world, EMOJI_COIN, spawn_pos);
        if (!coin) {
                return;
        }

        double side_dir = 1.0;

        coin->v = (Vec2){-side_dir * ATM_COIN_SIDE_SPEED, ATM_COIN_DOWN_SPEED};

        self->shake_distance -= ATM_COIN_DELAY;
}