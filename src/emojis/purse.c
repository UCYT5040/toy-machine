#include "emojis/purse.h"
#include "entities.h"
#include "recipes.h"
#include "vault.h"

#define PURSE_MAX_COINS 25
#define EJECT_INTERVAL 0.25;

void emoji_purse_update(World *world, Entity *self, double dt) {
        if (self->ejecting) {
                self->eject_timer -= dt;
                if (self->eject_timer <= 0.0) {
                        if (self->processing_input_count > 0) {
                                self->processing_input_count--;
                                int ejected_index =
                                    self->processing_inputs
                                        [self->processing_input_count];
                                Entity *ejected_entity =
                                    &world->entities[ejected_index];
                                ejected_entity->active = true;

                                double launch_offset =
                                    (self->definition->radius * self->scale) +
                                    (ejected_entity->definition->radius *
                                     ejected_entity->scale) +
                                    5.0;

                                ejected_entity->pos = vec2_add(
                                    self->pos,
                                    vec2_rotate((Vec2){launch_offset, 0.0},
                                                self->angle));
                                ejected_entity->last_pos = ejected_entity->pos;
                                ejected_entity->active = true;

                                // Apply a random spread
                                double spread =
                                    ((double)rand() / RAND_MAX - 0.5) * M_PI /
                                    6.0; // +/- 15 degrees
                                double launch_angle = self->angle + spread;
                                double launch_speed = 200.0; // Arbitrary speed
                                ejected_entity->v = vec2_rotate(
                                    (Vec2){launch_speed, 0.0}, launch_angle);

                                self->eject_timer +=
                                    EJECT_INTERVAL; // Adds instead of setting
                                                    // in case the timer is
                                                    // negative

                                vault_spend(&world->vault, 1);
                        }

                        if (self->processing_input_count == 0) {
                                self->ejecting = false;
                                free(self->processing_inputs);
                                self->processing_inputs = NULL;
                        }
                }
        }
}

#define MIN_THROW_VELOCITY 100.0

void emoji_purse_collide(World *world, Entity *self, Entity *other) {
        if (self->processing || self->ejecting ||
            other->tags & TAG_INVENTORY_IMMUNE) {
                return;
        }

        if (other->type == EMOJI_STONE) {
                // Enforce a minimum velocity to ensure the stone was thrown

                double velocity_magnitude = vec2_length(other->v);
                if (velocity_magnitude >= MIN_THROW_VELOCITY) {

                        // Drop (throw) all processing inputs!
                        self->ejecting = true;
                        self->eject_timer = 0.0;
                }
                return;
        }

        if (other->tags & TAG_INVENTORY && other->needs_coins > 0) {
                // If the other inventory needs coins, and this purse has coins,
                // transfer them
                if (self->processing_input_count > 0) {
                        int coins_to_transfer = fmin(
                            self->processing_input_count, other->needs_coins);

                        for (int i = 0; i < coins_to_transfer; i++) {
                                self->processing_input_count--;
                                int coin_index =
                                    self->processing_inputs
                                        [self->processing_input_count];
                                Entity *coin_entity =
                                    &world->entities[coin_index];

                                coin_entity->active = true;

                                // Transfer the coin to the other inventory
                                other->processing_inputs = realloc(
                                    other->processing_inputs,
                                    sizeof(int) *
                                        (other->processing_input_count + 1));
                                other->processing_inputs
                                    [other->processing_input_count] =
                                    coin_index;
                                other->processing_input_count++;
                        }
                }
                return;
        }

        if (other->type != EMOJI_COIN) {
                return;
        }

        if (self->processing_input_count >= PURSE_MAX_COINS) {
                return;
        }

        vault_add(&world->vault, 1);

        other->active = false;

        self->processing_inputs =
            realloc(self->processing_inputs,
                    sizeof(int) * (self->processing_input_count + 1));
        self->processing_inputs[self->processing_input_count] = other->index;
        self->processing_input_count++;
}
