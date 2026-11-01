#include "emojis/bank.h"

#include <math.h>
#include <stdbool.h>

#include "emoji_registry.h"
#include "entities.h"

#define BANK_SHAKE_SPEED 400.0 /* Minimum speed for the emoji to be moving for coin drop to trigger */
#define BANK_SHAKE_COOLDOWN 1.5 /* In seconds, after a coin drop, bank can't drop another coin until cooldown ends */
#define BANK_COIN_DOWN_SPEED 220.0 
#define BANK_COIN_SIDE_SPEED 80.0

static double vec2_length(Vec2 v) {
        /* a * a+b * b */
        return sqrt(v.x * v.x + v.y * v.y);
}

void emoji_bank_update(World *world, Entity *self, double dt) {
        if (self->shake_cooldown > 0.0) {
                self->shake_cooldown -= dt;
                /* 0 is the floor */
                if (self->shake_cooldown < 0.0) {
                        self->shake_cooldown = 0.0;
                }
        }

        if (world->dragged_entity_index != self->index) {
                self->last_pos = self->pos;
                self->shake_last_delta = (Vec2){0.0, 0.0};
                return;
        }

        Vec2 delta = {self->pos.x - self->last_pos.x,
                      self->pos.y - self->last_pos.y};
        self->last_pos = self->pos;

        if (dt <= 0.0) {
                return;
        }

        if (vec2_length(delta) / dt < BANK_SHAKE_SPEED) {
                return;
        }

        double dot = delta.x * self->shake_last_delta.x +
                     delta.y * self->shake_last_delta.y;
        bool reversed = dot < 0.0;
        self->shake_last_delta = delta;

        if (!reversed || self->shake_cooldown > 0.0) {
                return;
        }

        double bank_radius = self->definition->radius * self->scale;
        double coin_radius = EMOJI_DEFINITIONS[EMOJI_COIN].radius;
        Vec2 spawn_pos = {
                self->pos.x, self->pos.y + bank_radius + coin_radius + 2.0
        };

        Entity *coin = entity_spawn(world, EMOJI_COIN, spawn_pos);
        if (!coin) {
                return;
        }

        double side_dir = 0.0;
        if (delta.x > 0.0) {
                side_dir = 1.0;
        } else if (delta.x < 0.0) {
                side_dir = -1.0;
        }

        coin->v = (Vec2){-side_dir * BANK_COIN_SIDE_SPEED, 
                         BANK_COIN_DOWN_SPEED};

        self->shake_cooldown = BANK_SHAKE_COOLDOWN;
}