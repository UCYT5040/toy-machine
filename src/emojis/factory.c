#include "emojis/factory.h"
#include "entities.h"
#include "recipes.h"
#include "vault.h"

#define EJECT_INTERVAL 0.25

void emoji_factory_update(World *world, Entity *self, double dt) {
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
                        }

                        if (self->processing_input_count == 0) {
                                self->ejecting = false;
                                free(self->processing_inputs);
                                self->processing_inputs = NULL;
                        }
                }
        }

        if (self->processing && self->processing_timer > 0.0) {
                self->processing_timer -= dt;
                if (self->processing_timer <= 0.0) {
                        self->processing = false;

                        int new_entities = 1;

                        for (int i = 0; i < self->processing_input_count; i++) {
                                Entity *input_entity =
                                    &world
                                         ->entities[self->processing_inputs[i]];
                                if (input_entity->type == EMOJI_COIN ||
                                    input_entity->type == EMOJI_DOLLAR) {
                                        input_entity->collect = true;
                                } else {
                                        new_entities++;
                                }
                        }

                        int *new_entity_indices =
                            malloc(sizeof(int) * new_entities);
                        EmojiType new_emoji = recipe_resolve_output(
                            self->current_recipe,
                            &world->entities[self->processing_inputs[0]],
                            &world->entities[self->processing_inputs[1]]);

                        if (new_emoji > EMOJI_NONE && new_emoji < EMOJI_COUNT) {
                                EMOJI_DEFINITIONS[new_emoji].unlocked = true;

                                /* TODO: Probably should use a macro for the
                                 * amount added */
                                vault_add(&world->vault, 10);
                        }

                        Entity *new_entity =
                            entity_spawn(world, new_emoji, self->pos);
                        new_entity->active = false;

                        int next_index = 0;

                        new_entity_indices[next_index++] = new_entity->index;

                        free(self->processing_inputs);
                        self->processing_inputs = new_entity_indices;
                        self->processing_input_count = next_index;

                        self->ejecting = true;
                        self->eject_timer = 0.0;
                }
        }
}

#define MIN_THROW_VELOCITY 400.0

void emoji_factory_collide(World *world, Entity *self, Entity *other) {
        if (self->processing || self->ejecting ||
            other->type == EMOJI_FACTORY ||
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

                        return;
                }
        }

        other->active = false;

        self->processing_inputs =
            realloc(self->processing_inputs,
                    sizeof(int) * (self->processing_input_count + 1));
        self->processing_inputs[self->processing_input_count] = other->index;
        self->processing_input_count++;

        Entity *in_a = NULL;
        Entity *in_b = NULL;
        int coin_count = 0;

        if (self->processing_input_count >= 2) {
                for (int i = 0; i < self->processing_input_count; i++) {
                        Entity *input_entity =
                            &world->entities[self->processing_inputs[i]];
                        if (input_entity->type == EMOJI_COIN) {
                                coin_count++;
                        } else if (input_entity->type == EMOJI_DOLLAR) {
                                coin_count += 10;
                        } else if (in_a == NULL) {
                                in_a = input_entity;
                        } else if (in_b == NULL) {
                                in_b = input_entity;
                        }
                }

                if (in_a && in_b) {

                        const Entity *out_a = NULL;
                        const Entity *out_b = NULL;
                        const Recipe *match =
                            recipe_find_match(in_a, in_b, &out_a, &out_b);

                        if (match && match->coins_required <= coin_count) {
                                self->current_recipe = match;
                        } else {
                                self->current_recipe = NULL;
                        }

                        if (self->current_recipe) {
                                if (out_a !=
                                    &world->entities
                                         [self->processing_inputs[0]]) {
                                        /* Swap indexes 0 and 1 */
                                        int temp = self->processing_inputs[0];
                                        self->processing_inputs[0] =
                                            self->processing_inputs[1];
                                        self->processing_inputs[1] = temp;
                                }

                                self->processing = true;
                                self->processing_timer =
                                    self->current_recipe->processing_time;
                        } else {
                                // TODO: Show an error symbol
                        }
                }
        }
}
