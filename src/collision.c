#include "collision.h"
#include "entities.h"
#include <math.h>

static bool test_circle_vs_circle(Vec2 c1, double r1, Vec2 c2, double r2,
                                  CollisionManifold *out_manifold) {
        Vec2 delta = vec2_sub(c2, c1);
        double distance_sq = vec2_length_squared(delta);
        double radius_sum = r1 + r2;

        if (distance_sq > radius_sum * radius_sum) {
                return false; // Manifold will be set to colliding = false by
                              // the caller
        }

        double distance = fmax(sqrt(distance_sq), 0.0001);
        Vec2 normal = vec2_scale(delta, 1.0 / distance);
        double penetration = radius_sum - distance;
        Vec2 contact_point = vec2_add(c1, vec2_scale(normal, r1));

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
        Vec2 delta = vec2_sub(circle_center, box_center);
        Vec2 local = vec2_rotate(delta, -box_angle);

        double hw = box_width / 2.0;
        double hh = box_height / 2.0;

        Vec2 closest = {fmax(-hw, fmin(local.x, hw)), fmax(-hh, fmin(local.y, hh))};
        Vec2 diff = vec2_sub(local, closest);
        double diff_sq = vec2_length_squared(diff);

        double penetration;
        Vec2 local_normal;

        if (diff_sq < 0.000001) {
                double dist_left = local.x - (-hw);
                double dist_right = hw - local.x;
                double dist_top = local.y - (-hh);
                double dist_bottom = hh - local.y;

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
                closest = vec2_sub(local, vec2_scale(local_normal, min_dist));
        } else {
                if (diff_sq > circle_radius * circle_radius) {
                        return false;
                }

                double distance = fmax(sqrt(diff_sq), 0.0001);
                penetration = circle_radius - distance;
                local_normal = vec2_scale(diff, 1.0 / distance);
        }

        Vec2 world_normal = vec2_rotate(local_normal, box_angle);
        Vec2 world_closest = vec2_add(box_center, vec2_rotate(closest, box_angle));

        if (out_manifold) {
                *out_manifold = (CollisionManifold){
                    .colliding = true,
                    .penetration = penetration,
                    .normal = world_normal,
                    .contact_point = world_closest};
        }

        return true;
}

void get_box_corners(Vec2 center, double hw, double hh, double angle,
                     Vec2 out_corners[4]) {
        Vec2 local_corners[4] = {
            {-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}};

        for (int i = 0; i < 4; i++) {
                out_corners[i] =
                    vec2_add(center, vec2_rotate(local_corners[i], angle));
        }
}

static void project_box_on_axis(const Vec2 corners[4], Vec2 axis,
                                double *out_min, double *out_max) {
        double min_proj = vec2_dot(corners[0], axis);
        double max_proj = min_proj;

        for (int i = 1; i < 4; i++) {
                double proj = vec2_dot(corners[i], axis);
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

        Vec2 axes[4] = {
            vec2_rotate((Vec2){1.0, 0.0}, angle_a),
            vec2_rotate((Vec2){0.0, 1.0}, angle_a),
            vec2_rotate((Vec2){1.0, 0.0}, angle_b),
            vec2_rotate((Vec2){0.0, 1.0}, angle_b)};

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

        Vec2 ab = vec2_sub(center_b, center_a);
        if (vec2_dot(ab, best_axis) < 0) {
                best_axis = vec2_scale(best_axis, -1.0);
        }

        Vec2 contact =
            vec2_add(center_a, vec2_scale(best_axis, width_a / 2.0));

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
                Vec2 box_center = vec2_add(
                    e1->pos,
                    vec2_scale(vec2_rotate(c1->offset, e1->angle), e1->scale));
                return test_box_vs_circle(
                    box_center, c1->box.width * e1->scale,
                    c1->box.height * e1->scale, e1->angle + c1->angle, e2->pos,
                    c2->circle.radius * e2->scale, out_manifold);
        } else if (c1->type == COLLIDER_CIRCLE && c2->type == COLLIDER_BOX) {
                Vec2 box_center = vec2_add(
                    e2->pos,
                    vec2_scale(vec2_rotate(c2->offset, e2->angle), e2->scale));
                if (test_box_vs_circle(
                        box_center, c2->box.width * e2->scale,
                        c2->box.height * e2->scale, e2->angle + c2->angle,
                        e1->pos, c1->circle.radius * e1->scale, out_manifold)) {
                        if (out_manifold) {
                                out_manifold->normal =
                                    vec2_scale(out_manifold->normal, -1.0);
                        }
                        return true;
                }
        } else if (c1->type == COLLIDER_BOX && c2->type == COLLIDER_BOX) {
                Vec2 box_center1 = vec2_add(
                    e1->pos,
                    vec2_scale(vec2_rotate(c1->offset, e1->angle), e1->scale));
                Vec2 box_center2 = vec2_add(
                    e2->pos,
                    vec2_scale(vec2_rotate(c2->offset, e2->angle), e2->scale));

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

        double dist_sq = vec2_distance_squared(e1->pos, e2->pos);
        if (dist_sq > (r1 + r2) * (r1 + r2)) {
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

        Vec2 shift1 =
            vec2_scale(manifold->normal, manifold->penetration * e1share);
        Vec2 shift2 =
            vec2_scale(manifold->normal, manifold->penetration * e2share);
        e1->pos = vec2_sub(e1->pos, shift1);
        e2->pos = vec2_add(e2->pos, shift2);
}

bool get_lowest_point(Entity *entity, Vec2 *out_point, double *out_penetration,
                      double floor_y) {
        double max_y = -INFINITY;
        Vec2 lowest_point = {0.0, 0.0};

        for (int i = 0; i < MAX_COLLIDERS; i++) {
                Collider *collider = &entity->definition->colliders[i];
                if (collider->type == COLLIDER_NONE)
                        continue;

                Vec2 col_offset = vec2_scale(
                    vec2_rotate(collider->offset, entity->angle), entity->scale);
                Vec2 collider_center = vec2_add(entity->pos, col_offset);

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
                        Vec2 local_corners[4] = {
                            {-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}};

                        for (int j = 0; j < 4; j++) {
                                Vec2 pt = vec2_add(collider_center,
                                                   vec2_rotate(local_corners[j], total_angle));
                                if (pt.y > max_y) {
                                        max_y = pt.y;
                                        lowest_point = pt;
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