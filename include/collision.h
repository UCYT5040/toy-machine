#pragma once

#include <stdbool.h>
#include "types.h"

typedef struct {
        bool colliding;

        Vec2 normal; // Pointing from e1 to e2

        double penetration;

        Vec2 contact_point;
} CollisionManifold;

bool entity_is_colliding(Entity *e1, Entity *e2,
                         CollisionManifold *out_manifold);

void entity_fix_overlap(Entity *e1, Entity *e2,
                        const CollisionManifold *manifold);

bool collision_get_lowest_point(Entity *entity, Vec2 *out_point,
                                double *out_penetration, double floor_y);

void get_box_corners(Vec2 center, double hw, double hh, double angle,
                     Vec2 out_corners[4]);

bool get_lowest_point(Entity *entity, Vec2 *out_point, double *out_penetration,
                      double floor_y);