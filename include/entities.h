#pragma once

#include <stdbool.h>

#include "world.h"

static const double G = 6.67430e-11; /* m^3 / (kg · s^2) */

void entity_create_default(Entity *entity, int definition_index, Vec2 pos,
                           int entity_index);

void entity_physics_step(Entity *entity);

void entity_physics_integrate(Entity *entity, double *dt);