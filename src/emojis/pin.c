#include "emojis/pin.h"

#define MIN_THROW_VELOCITY 100.0

void emoji_pin_unpin(World *world, Entity *pin) {
        if (!pin->pinned)
                return;

        Entity *target = &world->entities[pin->pinned_target_index];

        if (target->pinned_by_index == pin->index) {
                target->pinned = false;
                target->pinned_by_index = -1;
        }

        pin->pinned = false;
        pin->pinned_target_index = -1;

        double spread =
            ((double)rand() / RAND_MAX - 0.5) * M_PI / 6.0; // +/- 15 degrees
        double launch_angle = pin->angle + spread;
        pin->v = vec2_rotate((Vec2){200.0, 0.0}, launch_angle);
}

void emoji_pin_update(World *world, Entity *self, double dt) {
        (void)dt;

        if (self->pinned_target_index >= 0) {
                Entity *target = &world->entities[self->pinned_target_index];

                if (!target->active || target->pinned_by_index != self->index) {
                        emoji_pin_unpin(world, self);
                        return;
                }

                self->pos = vec2_add(target->pos, self->pinned_offset);
        }
}

void emoji_pin_collide(World *world, Entity *self, Entity *other) {
        if (self->pinned_target_index >= 0) {
                if (other->type == EMOJI_STONE) {
                        double velocity_magnitude = vec2_length(other->v);
                        if (velocity_magnitude >= MIN_THROW_VELOCITY) {
                                emoji_pin_unpin(world, self);
                        }
                }
                return;
        }

        if (other->type == EMOJI_PIN || other->pinned || !other->active) {
                return;
        }

        self->pinned = true;
        self->pinned_target_index = other->index;
        self->pinned_offset = vec2_sub(self->pos, other->pos);
        self->v = (Vec2){0.0, 0.0};
        self->a = (Vec2){0.0, 0.0};
        self->omega = 0.0;

        other->pinned = true;
        other->pinned_by_index = self->index;
        other->v = (Vec2){0.0, 0.0};
        other->a = (Vec2){0.0, 0.0};
        other->omega = 0.0;

        if (world->pressed_entity_index == self->index) {
                world->pressed_entity_index = -1;
        }
}