#pragma once

#include <stdbool.h>

#include "world.h"

static const double G = 6.67430e-11; /* m^3 / (kg · s^2) */

void entity_create_default(Entity *entity, int definition_index, Vec2 pos,
                           int entity_index);

Entity *entity_spawn(World *world, EmojiType type, Vec2 pos);

void entity_physics_step(Entity *entity, World *world);

void entity_physics_integrate(Entity *entity, double *dt);

void entity_constrain_to_bounds(Entity *entity, World *world);

typedef struct {
        bool colliding;

        Vec2 normal; // Pointing from e1 to e2

        double penetration;

        Vec2 contact_point;
} CollisionManifold;