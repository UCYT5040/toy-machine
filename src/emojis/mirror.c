#include "emojis/mirror.h"
#include "SDL_timer.h"
#include "entities.h"
#include "vec2.h"
#include "world.h"

#include <SDL2/SDL.h>
#include <math.h>

void emoji_mirror_collide(World *world, Entity *self, Entity *other) {
        struct emoji_mirror_ctx *ctx =
            (struct emoji_mirror_ctx *)self->custom_data;

        int now = SDL_GetTicks();

        if (now - ctx->last_action < MIRROR_DUPLICATION_TIMEOUT)
                return;

        Entity *new = entity_spawn(world, other->type, other->pos);
        new->v = vec2_rotate(other->v, M_PI);

        ctx->last_action = now;
}