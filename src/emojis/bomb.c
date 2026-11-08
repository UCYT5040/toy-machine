#include "emojis/bomb.h"
#include "entities.h"
#include "types.h"
#include "vec2.h"
#include "world.h"

#include <SDL2/SDL.h>
#include <math.h>

static void emoji_bomb_explode(World *world, Entity *self) {
        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *other = &world->entities[i];
                if (!other->active || other == self)
                        continue;

                double dx = other->pos.x - self->pos.x;
                double dy = other->pos.y - self->pos.y;
                double distance = sqrt(dx * dx + dy * dy);
                if (distance <= 1.0) {
                        other->v =
                            vec2_scale(vec2_normalize(other->v),
                                       vec2_length(other->v) + BOMB_FORCE);
                        continue;
                }

                double dir = atan2(dy, dx);
                Vec2 push = (Vec2){1.0, 0.0};
                push = vec2_scale(push, BOMB_FORCE / distance);
                push = vec2_rotate(push, dir);
                other->v = vec2_add(other->v, push);
        }

        entity_remove(world, self);
}

void emoji_bomb_update(World *world, Entity *self, double dt) {
        (void)dt;

        struct emoji_bomb_ctx *ctx = (struct emoji_bomb_ctx *)self->custom_data;

        if (ctx->spawned == 0)
                ctx->spawned = SDL_GetTicks();

        if (SDL_GetTicks() - ctx->spawned >= BOMB_TIMER)
                emoji_bomb_explode(world, self);
}

void emoji_bomb_collide(World *world, Entity *self, Entity *other) {
        (void)other;

        emoji_bomb_explode(world, self);
}