#include "emojis/money_bag.h"
#include "entities.h"
#include "recipes.h"
#include "vault.h"
#define MONEY_BAG_MAX_DOLLARS 25
#define MONEY_BAG_MAX_COINS 10
#define EJECT_INTERVAL 0.25;

void emoji_money_bag_update(World *world, Entity *self, double dt) {
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

                                if (ejected_entity->type == EMOJI_COIN) {
                                        vault_spend(&world->vault, 1);
                                } else if (ejected_entity->type ==
                                           EMOJI_DOLLAR) {
                                        vault_spend(&world->vault, 10);
                                }
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

void emoji_money_bag_collide(World *world, Entity *self, Entity *other) {
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

        // TODO: Combine these into one loop
        int coin_count = 0;
        for (int i = 0; i < self->processing_input_count; i++) {
                int input_index = self->processing_inputs[i];
                Entity *input_entity = &world->entities[input_index];
                if (input_entity->type == EMOJI_COIN) {
                        coin_count++;
                }
        }
        int dollar_count = 0;
        for (int i = 0; i < self->processing_input_count; i++) {
                int input_index = self->processing_inputs[i];
                Entity *input_entity = &world->entities[input_index];
                if (input_entity->type == EMOJI_DOLLAR) {
                        dollar_count++;
                }
        }

        if (other->type == EMOJI_COIN) {
                if (coin_count >= MONEY_BAG_MAX_COINS) {
                        return;
                }

                vault_add(&world->vault, 1);

                other->active = false;

                self->processing_inputs =
                    realloc(self->processing_inputs,
                            sizeof(int) * (self->processing_input_count + 1));
                self->processing_inputs[self->processing_input_count] =
                    other->index;
                self->processing_input_count++;

                if (++coin_count >= 10) {
                        // Convert to a dollar if there is room
                        if (dollar_count + 1 <= MONEY_BAG_MAX_DOLLARS) {
                                // Remove the 10 coins and add a dollar
                                int coins_to_remove = 10;
                                for (int i = 0;
                                     i < self->processing_input_count; i++) {
                                        int input_index =
                                            self->processing_inputs[i];
                                        Entity *input_entity =
                                            &world->entities[input_index];
                                        if (input_entity->type == EMOJI_COIN) {
                                                input_entity->collect = true;
                                                self->processing_inputs[i] = -1;
                                                coins_to_remove--;
                                                if (coins_to_remove == 0) {
                                                        break;
                                                }
                                        }
                                }

                                // Remove the -1 entries from processing_inputs
                                int new_count = 0;
                                for (int i = 0;
                                     i < self->processing_input_count; i++) {
                                        if (self->processing_inputs[i] != -1) {
                                                self->processing_inputs
                                                    [new_count++] =
                                                    self->processing_inputs[i];
                                        }
                                }
                                self->processing_input_count = new_count;

                                // Add a dollar
                                Entity *dollar_entity = entity_spawn(
                                    world, EMOJI_DOLLAR, self->pos);
                                dollar_entity->active = false;

                                self->processing_inputs = realloc(
                                    self->processing_inputs,
                                    sizeof(int) *
                                        (self->processing_input_count + 1));
                                self->processing_inputs
                                    [self->processing_input_count] =
                                    dollar_entity->index;
                                self->processing_input_count++;
                        }
                }
        }

        if (other->type == EMOJI_DOLLAR) {

                if (dollar_count >= MONEY_BAG_MAX_DOLLARS) {
                        if (coin_count + 10 <= MONEY_BAG_MAX_COINS) {
                                /* Remove the dollar and add 10 coins */
                                other->active = false;
                                other->collect = true;

                                self->processing_inputs = realloc(
                                    self->processing_inputs,
                                    sizeof(int) *
                                        (self->processing_input_count + 10));
                                self->processing_input_count += 10;
                                for (int i = 0; i < 10; i++) {
                                        self->processing_inputs[i] =
                                            entity_spawn(world, EMOJI_COIN,
                                                         other->pos)
                                                ->index;
                                        world
                                            ->entities
                                                [self->processing_inputs[i]]
                                            .active = false;
                                }

                                vault_add(&world->vault, 10);
                        }
                        return;
                }

                vault_add(&world->vault, 10);

                other->active = false;
                self->processing_inputs =
                    realloc(self->processing_inputs,
                            sizeof(int) * (self->processing_input_count + 1));
                self->processing_inputs[self->processing_input_count] =
                    other->index;
                self->processing_input_count++;
        }
}
