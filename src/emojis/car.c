#include "emojis/car.h"
#include "vec2.h"
#include "world.h"

#include <math.h>
#include <stdio.h>

Vec2 last_wall_pos = (Vec2){-1.0, -1.0};
Vec2 last_wall_normal = (Vec2){0.0, 0.0};

void emoji_car_wall_collide(World *world, Entity *self, Vec2 normal) {
        (void)world;

        last_wall_pos = self->pos;
        last_wall_normal = normal;
}

void emoji_car_update(World *world, Entity *self, double dt) {
        if (last_wall_pos.x < 0.0 || last_wall_pos.y < 0.0)
                return;

        Vec2 up = vec2_rotate((Vec2){0.0, -1.0}, self->angle);
        if (vec2_dot(up, last_wall_normal) >=
                cos(M_PI / 18.0 /* Around 10° */) &&
            vec2_distance(last_wall_pos, self->pos) <= 1.0) {
                Vec2 car_speed =
                    vec2_scale((Vec2){up.y, -up.x}, CAR_ACCELERATION * dt);

                self->v = vec2_add(self->v, car_speed);
        }
}