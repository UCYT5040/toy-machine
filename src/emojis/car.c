#include "emojis/car.h"
#include "vec2.h"
#include "world.h"

#include <math.h>

void emoji_car_wall_collide(World *world, Entity *self, Vec2 normal) {
        (void)world;

        struct emoji_car_ctx *ctx = (struct emoji_car_ctx *)self->custom_data;

        ctx->last_wall_pos = self->pos;
        ctx->last_wall_normal = normal;
        if (!ctx->inited)
                ctx->inited = true;
}

void emoji_car_update(World *world, Entity *self, double dt) {
        (void)world;

        struct emoji_car_ctx *ctx = (struct emoji_car_ctx *)self->custom_data;
        if (!ctx->inited)
                return;

        Vec2 up = vec2_rotate((Vec2){0.0, -1.0}, self->angle);
        if (vec2_dot(up, ctx->last_wall_normal) >=
                cos(M_PI / 18.0 /* Around 10° */) &&
            vec2_distance(ctx->last_wall_pos, self->pos) <= 1.0) {
                Vec2 car_speed =
                    vec2_scale((Vec2){up.y, -up.x}, CAR_ACCELERATION * dt);

                self->v = vec2_add(self->v, car_speed);
        }
}