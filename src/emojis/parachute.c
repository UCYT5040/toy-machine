#include "emojis/parachute.h"

#define MIN_THROW_VELOCITY 100.0
#define TERMINAL_FLOAT_SPEED 60.0

static double lerp(double a, double b, double t) { return a + (b - a) * t; }

void emoji_parachute_update(World *world, Entity *self, double dt) {
        (void)dt;

        if (self->pinned_target_index >= 0) {
                Entity *target = &world->entities[self->pinned_target_index];

                if (!target->active) {
                        self->pinned_target_index = -1;
                        return;
                }

                if (target->v.y > TERMINAL_FLOAT_SPEED) {
                        target->v.y =
                            lerp(target->v.y, TERMINAL_FLOAT_SPEED, dt * 8.0);
                }
                target->v.x *=
                    pow(0.95, dt * 60.0); // slight horizontal dampening

                Vec2 up = {0.0, -1.0};
                if (vec2_length_squared(target->v) > 10.0) {
                        up = vec2_scale(vec2_normalize(target->v), -1.0);
                }

                self->angle = atan2(up.x, -up.y);

                Vec2 r = vec2_rotate(self->pinned_offset, target->angle);

                Vec2 pivot_pos = vec2_add(target->pos, r);

                Vec2 lift_force = {0.0, -target->definition->mass * 981.0};

                target->torque += vec2_cross(r, lift_force);
                target->omega *= 0.96;

                self->pos = vec2_add(
                    pivot_pos,
                    vec2_scale(up, self->definition->radius * self->scale));

                self->v = target->v;
        }
}

void emoji_parachute_collide(World *world, Entity *self, Entity *other) {
        if (self->pinned_target_index >= 0) {
                if (other->type == EMOJI_STONE) {
                        double velocity_magnitude = vec2_length(other->v);
                        if (velocity_magnitude >= MIN_THROW_VELOCITY) {
                                self->pinned_target_index = -1;
                        }
                }
                return;
        }

        if (other->type == EMOJI_PIN || other->pinned || !other->active) {
                return;
        }

        self->pinned_target_index = other->index;

        Vec2 to_parachute = vec2_sub(self->pos, other->pos);
        Vec2 local_dir =
            vec2_rotate(vec2_normalize(to_parachute), -other->angle);

        self->pinned_offset =
            vec2_scale(local_dir, other->definition->radius * other->scale);
        self->v = (Vec2){0.0, 0.0};
        self->a = (Vec2){0.0, 0.0};
        self->omega = 0.0;

        if (world->pressed_entity_index == self->index) {
                world->pressed_entity_index = -1;
        }
}