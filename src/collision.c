#include "collision.h"
#include "world.h"
#include <math.h>

static bool test_circle_vs_circle(Vec2 c1, double r1, Vec2 c2, double r2,
                                  CollisionManifold *out_manifold) {
        double dx = c2.x - c1.x;
        double dy = c2.y - c1.y;
        double distance_sq = dx * dx + dy * dy;
        double radius_sum = r1 + r2;

        if (distance_sq > radius_sum * radius_sum) {
                return false; // Manifold will be set to colliding = false by
                              // the caller
        }

        double distance = fmax(sqrt(distance_sq), 0.0001);
        Vec2 normal = {dx / distance, dy / distance};
        double penetration = radius_sum - distance;
        Vec2 contact_point = {c1.x + normal.x * r1, c1.y + normal.y * r1};

        if (out_manifold) {
                *out_manifold =
                    (CollisionManifold){.colliding = true,
                                        .normal = normal,
                                        .penetration = penetration,
                                        .contact_point = contact_point};
        }
        return true;
}

static bool test_box_vs_circle(Vec2 box_center, double box_width,
                               double box_height, double box_angle,
                               Vec2 circle_center, double circle_radius,
                               CollisionManifold *out_manifold) {
        double dx = circle_center.x - box_center.x;
        double dy = circle_center.y - box_center.y;

        double local_x = dx * cos(box_angle) + dy * sin(box_angle);
        double local_y = -dx * sin(box_angle) + dy * cos(box_angle);

        double hw = box_width / 2.0;
        double hh = box_height / 2.0;

        double closest_x = fmax(-hw, fmin(local_x, hw));
        double closest_y = fmax(-hh, fmin(local_y, hh));

        double diff_x = local_x - closest_x;
        double diff_y = local_y - closest_y;
        double diff_sq = diff_x * diff_x + diff_y * diff_y;

        double penetration;
        Vec2 local_normal;

        if (diff_sq < 0.000001) {
                double dist_left = local_x - (-hw);
                double dist_right = hw - local_x;
                double dist_top = local_y - (-hh);
                double dist_bottom = hh - local_y;

                double min_dist = dist_left;
                local_normal = (Vec2){-1.0, 0.0};

                if (dist_right < min_dist) {
                        min_dist = dist_right;
                        local_normal = (Vec2){1.0, 0.0};
                }
                if (dist_top < min_dist) {
                        min_dist = dist_top;
                        local_normal = (Vec2){0.0, -1.0};
                }
                if (dist_bottom < min_dist) {
                        min_dist = dist_bottom;
                        local_normal = (Vec2){0.0, 1.0};
                }
                penetration = circle_radius + min_dist;

                closest_x = local_x - local_normal.x * min_dist;
                closest_y = local_y - local_normal.y * min_dist;
        } else {
                if (diff_sq > circle_radius * circle_radius) {
                        return false;
                }

                double distance = fmax(sqrt(diff_sq), 0.0001);
                penetration = circle_radius - distance;
                local_normal = (Vec2){diff_x / distance, diff_y / distance};
        }

        double world_normal_x =
            local_normal.x * cos(box_angle) - local_normal.y * sin(box_angle);
        double world_normal_y =
            local_normal.x * sin(box_angle) + local_normal.y * cos(box_angle);

        double world_closest_x =
            closest_x * cos(box_angle) - closest_y * sin(box_angle);
        double world_closest_y =
            closest_x * sin(box_angle) + closest_y * cos(box_angle);

        if (out_manifold) {
                *out_manifold = (CollisionManifold){
                    .colliding = true,
                    .penetration = penetration,
                    .normal = (Vec2){world_normal_x, world_normal_y},
                    .contact_point = (Vec2){box_center.x + world_closest_x,
                                            box_center.y + world_closest_y}};
        }

        return true;
}

void get_box_corners(Vec2 center, double hw, double hh, double angle,
                     Vec2 out_corners[4]) {
        double cos_a = cos(angle);
        double sin_a = sin(angle);

        double local_corners[4][2] = {
            {-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}};

        for (int i = 0; i < 4; i++) {
                out_corners[i].x = center.x + local_corners[i][0] * cos_a -
                                   local_corners[i][1] * sin_a;
                out_corners[i].y = center.y + local_corners[i][0] * sin_a +
                                   local_corners[i][1] * cos_a;
        }
}

static void project_box_on_axis(const Vec2 corners[4], Vec2 axis,
                                double *out_min, double *out_max) {
        double min_proj = corners[0].x * axis.x + corners[0].y * axis.y;
        double max_proj = min_proj;

        for (int i = 1; i < 4; i++) {
                double proj = corners[i].x * axis.x + corners[i].y * axis.y;
                if (proj < min_proj)
                        min_proj = proj;
                if (proj > max_proj)
                        max_proj = proj;
        }

        *out_min = min_proj;
        *out_max = max_proj;
}

static bool test_box_vs_box(Vec2 center_a, double width_a, double height_a,
                            double angle_a, Vec2 center_b, double width_b,
                            double height_b, double angle_b,
                            CollisionManifold *out_manifold) {
        Vec2 corners_a[4];
        Vec2 corners_b[4];
        get_box_corners(center_a, width_a / 2.0, height_a / 2.0, angle_a,
                        corners_a);
        get_box_corners(center_b, width_b / 2.0, height_b / 2.0, angle_b,
                        corners_b);

        Vec2 axes[4] = {{cos(angle_a), sin(angle_a)},
                        {-sin(angle_a), cos(angle_a)},
                        {cos(angle_b), sin(angle_b)},
                        {-sin(angle_b), cos(angle_b)}};

        double min_overlap = INFINITY;
        Vec2 best_axis = {0.0, 0.0};

        for (int i = 0; i < 4; i++) {
                Vec2 axis = axes[i];
                double min_a, max_a, min_b, max_b;

                project_box_on_axis(corners_a, axis, &min_a, &max_a);
                project_box_on_axis(corners_b, axis, &min_b, &max_b);

                double overlap = fmin(max_a, max_b) - fmax(min_a, min_b);
                if (overlap <= 0.0) {
                        return false; // Separating axis found
                }

                if (overlap < min_overlap) {
                        min_overlap = overlap;
                        best_axis = axis;
                }
        }

        Vec2 ab = {center_b.x - center_a.x, center_b.y - center_a.y};
        if (ab.x * best_axis.x + ab.y * best_axis.y < 0) {
                best_axis.x = -best_axis.x;
                best_axis.y = -best_axis.y;
        }

        Vec2 contact = {center_a.x + best_axis.x * (width_a / 2.0),
                        center_a.y + best_axis.y * (height_a / 2.0)};

        if (out_manifold) {
                *out_manifold = (CollisionManifold){.colliding = true,
                                                    .normal = best_axis,
                                                    .penetration = min_overlap,
                                                    .contact_point = contact};
        }

        return true;
}

static bool test_collider_pair(Entity *e1, Collider *c1, Entity *e2,
                               Collider *c2, CollisionManifold *out_manifold) {
        if (c1->type == COLLIDER_CIRCLE && c2->type == COLLIDER_CIRCLE) {
                return test_circle_vs_circle(
                    e1->pos, e1->definition->radius * e1->scale, e2->pos,
                    e2->definition->radius * e2->scale, out_manifold);
        } else if (c1->type == COLLIDER_BOX && c2->type == COLLIDER_CIRCLE) {
                double cos_a = cos(e1->angle);
                double sin_a = sin(e1->angle);
                Vec2 box_center = {
                    e1->pos.x + c1->offset.x * cos_a - c1->offset.y * sin_a,
                    e1->pos.y + c1->offset.x * sin_a + c1->offset.y * cos_a};
                return test_box_vs_circle(
                    box_center, c1->box.width * e1->scale,
                    c1->box.height * e1->scale, e1->angle + c1->angle, e2->pos,
                    c2->circle.radius * e2->scale, out_manifold);
        } else if (c1->type == COLLIDER_CIRCLE && c2->type == COLLIDER_BOX) {
                double cos_a = cos(e2->angle);
                double sin_a = sin(e2->angle);
                Vec2 box_center = {
                    e2->pos.x + c2->offset.x * cos_a - c2->offset.y * sin_a,
                    e2->pos.y + c2->offset.x * sin_a + c2->offset.y * cos_a};
                if (test_box_vs_circle(
                        box_center, c2->box.width * e2->scale,
                        c2->box.height * e2->scale, e2->angle + c2->angle,
                        e1->pos, c1->circle.radius * e1->scale, out_manifold)) {
                        if (out_manifold) {
                                out_manifold->normal.x *= -1.0;
                                out_manifold->normal.y *= -1.0;
                        }
                        return true;
                }
        } else if (c1->type == COLLIDER_BOX && c2->type == COLLIDER_BOX) {
                double cos_a1 = cos(e1->angle);
                double sin_a1 = sin(e1->angle);
                Vec2 box_center1 = {
                    e1->pos.x + c1->offset.x * cos_a1 - c1->offset.y * sin_a1,
                    e1->pos.y + c1->offset.x * sin_a1 + c1->offset.y * cos_a1};

                double cos_a2 = cos(e2->angle);
                double sin_a2 = sin(e2->angle);
                Vec2 box_center2 = {
                    e2->pos.x + c2->offset.x * cos_a2 - c2->offset.y * sin_a2,
                    e2->pos.y + c2->offset.x * sin_a2 + c2->offset.y * cos_a2};

                return test_box_vs_box(box_center1, c1->box.width * e1->scale,
                                       c1->box.height * e1->scale,
                                       e1->angle + c1->angle, box_center2,
                                       c2->box.width * e2->scale,
                                       c2->box.height * e2->scale,
                                       e2->angle + c2->angle, out_manifold);
        }
        return false;
}

bool entity_is_colliding(Entity *e1, Entity *e2,
                         CollisionManifold *out_manifold) {
        double r1 = e1->definition->radius * e1->scale;
        double r2 = e2->definition->radius * e2->scale;

        double dx = e2->pos.x - e1->pos.x;
        double dy = e2->pos.y - e1->pos.y;

        if (dx * dx + dy * dy > (r1 + r2) * (r1 + r2)) {
                if (out_manifold) {
                        *out_manifold = (CollisionManifold){.colliding = false};
                }
                return false;
        }

        bool hit = false;
        CollisionManifold deepest = {.penetration = -1.0};

        for (int i = 0; i < MAX_COLLIDERS; i++) {
                for (int j = 0; j < MAX_COLLIDERS; j++) {
                        CollisionManifold manifold;
                        if (test_collider_pair(
                                e1, &e1->definition->colliders[i], e2,
                                &e2->definition->colliders[j], &manifold)) {
                                hit = true;
                                if (manifold.penetration >
                                    deepest.penetration) {
                                        deepest = manifold;
                                }
                        }
                }
        }

        if (hit) {
                *out_manifold = deepest;
        } else {
                *out_manifold = (CollisionManifold){.colliding = false};
        }
        return hit;
}

void entity_fix_overlap(Entity *e1, Entity *e2,
                        const CollisionManifold *manifold) {
        if (e1->pinned && e2->pinned) {
                return; // Both entities are pinned, do not move them
        }

        double total_mass = e1->definition->mass + e2->definition->mass;
        if (total_mass <= 0.0)
                return; // Both are immovable
        double e1share =
            e1->pinned ? 0.0
                       : (e2->pinned ? 1.0 : e2->definition->mass / total_mass);
        double e2share =
            e2->pinned ? 0.0
                       : (e1->pinned ? 1.0 : e1->definition->mass / total_mass);

        e1->pos.x -= manifold->normal.x * manifold->penetration * e1share;
        e1->pos.y -= manifold->normal.y * manifold->penetration * e1share;
        e2->pos.x += manifold->normal.x * manifold->penetration * e2share;
        e2->pos.y += manifold->normal.y * manifold->penetration * e2share;
}

bool get_lowest_point(Entity *entity, Vec2 *out_point, double *out_penetration,
                      double floor_y) {
        double max_y = -INFINITY;
        Vec2 lowest_point = {0.0, 0.0};

        double cos_e = cos(entity->angle);
        double sin_e = sin(entity->angle);

        for (int i = 0; i < MAX_COLLIDERS; i++) {
                Collider *collider = &entity->definition->colliders[i];
                if (collider->type == COLLIDER_NONE)
                        continue;

                Vec2 collider_center = {
                    entity->pos.x + (collider->offset.x * cos_e -
                                     collider->offset.y * sin_e) *
                                        entity->scale,
                    entity->pos.y + (collider->offset.x * sin_e +
                                     collider->offset.y * cos_e) *
                                        entity->scale};

                if (collider->type == COLLIDER_CIRCLE) {
                        double radius = collider->circle.radius * entity->scale;

                        double py = collider_center.y + radius;
                        if (py > max_y) {
                                max_y = py;
                                lowest_point = (Vec2){collider_center.x, py};
                        }
                } else if (collider->type == COLLIDER_BOX) {
                        double hw = (collider->box.width / 2.0) * entity->scale;
                        double hh =
                            (collider->box.height / 2.0) * entity->scale;

                        double total_angle = entity->angle + collider->angle;
                        double cos_a = cos(total_angle);
                        double sin_a = sin(total_angle);

                        double local_corners[4][2] = {
                            {-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}};

                        for (int j = 0; j < 4; j++) {
                                double px = collider_center.x +
                                            local_corners[j][0] * cos_a -
                                            local_corners[j][1] * sin_a;
                                double py = collider_center.y +
                                            local_corners[j][0] * sin_a +
                                            local_corners[j][1] * cos_a;
                                if (py > max_y) {
                                        max_y = py;
                                        lowest_point = (Vec2){px, py};
                                }
                        }
                }
        }

        if (max_y > floor_y) {
                *out_point = lowest_point;
                *out_penetration = max_y - floor_y;
                return true;
        } else {
                return false;
        }
}