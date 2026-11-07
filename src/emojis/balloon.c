#include "emojis/balloon.h"
#include "world.h"

#include <math.h>

void emoji_balloon_update(World *world, Entity *self, double dt) {
        (void)world;

        struct emoji_balloon_ctx *ctx =
            (struct emoji_balloon_ctx *)self->custom_data;

        if (ctx->swerve_dir >= 0.0) {
                self->v.x += 100.0 * dt * fmin(fabs(self->v.y) / 100.0, 1.0);
                ctx->swerve_val +=
                    100.0 * dt * fmin(fabs(self->v.y) / 100.0, 1.0);
                if (ctx->swerve_val >= 80.0)
                        ctx->swerve_dir = -1.0;
        } else {
                self->v.x -= 100.0 * dt * fmin(fabs(self->v.y) / 100.0, 1.0);
                ctx->swerve_val -=
                    100.0 * dt * fmin(fabs(self->v.y) / 100.0, 1.0);
                if (ctx->swerve_val <= -80.0)
                        ctx->swerve_dir = 1.0;
        }

        self->v.y += BALLOON_ACCELERATION * dt;

        self->angle *= 0.8; // It tries to point upwards
}