#include "emojis/factory.h"
#include "entities.h"
#include "recipes.h"

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
                        if (self->current_recipe->consume_a) {
                                // Mark to be collected
                                world->entities[self->processing_inputs[0]]
                                    .collect = true;
                        } else {
                                new_entities++;
                        }
                        if (self->current_recipe->consume_b) {
                                // Mark to be collected
                                world->entities[self->processing_inputs[1]]
                                    .collect = true;
                        } else {
                                new_entities++;
                        }

                        int *new_entity_indices =
                            malloc(sizeof(int) * new_entities);
                        EmojiType new_emoji = recipe_resolve_output(
                            self->current_recipe,
                            &world->entities[self->processing_inputs[0]],
                            &world->entities[self->processing_inputs[1]]);

                        Entity *new_entity =
                            entity_spawn(world, new_emoji, self->pos);
                        new_entity->active = false;

                        new_entity_indices[0] = new_entity->index;

                        // TODO: Make it so we just add back the input entity
                        // rather than creating a new one. This requires making
                        // sure overwriting entity slots requires more than just
                        // inactivity (e.g., a collection flag)
                        if (!self->current_recipe->consume_a) {
                                Entity *new_entity_a = entity_spawn(
                                    world,
                                    world->entities[self->processing_inputs[0]]
                                        .type,
                                    self->pos);
                                new_entity_a->active = false;

                                new_entity_indices[1] = new_entity_a->index;
                        }
                        if (!self->current_recipe->consume_b) {
                                Entity *new_entity_b = entity_spawn(
                                    world,
                                    world->entities[self->processing_inputs[1]]
                                        .type,
                                    self->pos);
                                new_entity_b->active = false;

                                new_entity_indices[2] = new_entity_b->index;
                        }

                        free(self->processing_inputs);
                        self->processing_inputs = new_entity_indices;
                        self->processing_input_count = new_entities;

                        self->ejecting = true;
                        self->eject_timer = 0.0;
                }
        }
}

#define MIN_THROW_VELOCITY 200.0

void emoji_factory_collide(World *world, Entity *self, Entity *other) {
        if (self->processing || self->ejecting ||
            other->type == EMOJI_FACTORY) {
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

        if (self->processing_input_count >= 2) {
                const Entity *out_a = NULL;
                const Entity *out_b = NULL;
                self->current_recipe = recipe_find_match(
                    &world->entities[self->processing_inputs[0]],
                    &world->entities[self->processing_inputs[1]], &out_a,
                    &out_b);

                if (self->current_recipe) {
                        if (out_a !=
                            &world->entities[self->processing_inputs[0]]) {
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
