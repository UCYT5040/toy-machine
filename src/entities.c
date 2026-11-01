#include <math.h>

#include "collision.h"
#include "emoji_registry.h"
#include "entities.h"
#include "world.h"

void entity_create_default(Entity *entity, int definition_index, Vec2 pos,
                           int entity_index) {
        entity->active = true;
        entity->collect = false;
        entity->definition = &EMOJI_DEFINITIONS[definition_index];
        entity->type = (EmojiType)definition_index;
        entity->pos = pos;
        entity->last_pos = pos;
        entity->shake_last_delta = (Vec2){0.0, 0.0};
        entity->shake_cooldown = 0.0;
        entity->despawn_timer = -1.0;
        entity->v = (Vec2){0.0, 0.0};
        entity->angle = 0.0;
        entity->scale = 1.0;
        entity->tags = entity->definition->default_tags;
        entity->index = entity_index;
        entity->a = (Vec2){0.0, 0.0};
        entity->pinned = false;
        entity->pinned_target_index = -1;
        entity->pinned_by_index = -1;
        entity->pinned_offset = (Vec2){0.0, 0.0};
}

Entity *entity_spawn(World *world, EmojiType type, Vec2 pos) {
        Entity *entity = NULL;
        for (int i = 0; i < MAX_ENTITIES; i++) {
                if (world->entities[i].collect) {
                        entity_create_default(&world->entities[i], type, pos,
                                              i);
                        entity = &world->entities[i];
                        break;
                }
        }

        if (entity) {
                // This should be recalculated each time scale changes. Consider
                // moving to a helper
                double mass = entity->definition->mass;
                if (mass <= 0.0 || entity->pinned)
                        entity->inv_inertia = 0.0;
                else {
                        double I; // should we rename this? since it's a capital
                                  // variable but not a constant
                        if (entity->definition->colliders[0].type ==
                            COLLIDER_BOX) {
                                double w =
                                    entity->definition->colliders[0].box.width *
                                    entity->scale;
                                double h = entity->definition->colliders[0]
                                               .box.height *
                                           entity->scale;
                                I = (1.0 / 12.0) * mass * (w * w + h * h);
                        } else {
                                double r = entity->definition->colliders[0]
                                               .circle.radius *
                                           entity->scale;
                                I = 0.5 * mass * r * r;
                        }
                        entity->inv_inertia = 1.0 / I;
                }
        }

        return entity; // No available entity slots
}

// Calculates the gravitational field strengh of `other` on `self`
static Vec2 entity_gravitational_field_strength(Entity *self, Entity *other) {
        double dx = other->pos.x - self->pos.x;
        double dy = other->pos.y - self->pos.y;
        double distance = fmax(sqrt(dx * dx + dy * dy), 0.01);

        double a = G * other->definition->mass / (distance * distance);

        return (Vec2){a * dx / distance, a * dy / distance};
}

// Negative velocity: Entities are moving away from eachother
// Positive velocity: Entities are moving towards eachother
// A collision is not guaranteed if entities are moving towards eachother
static double entity_relative_velocity(Entity *e1, Entity *e2) {
        double dx = e2->pos.x - e1->pos.x;
        double dy = e2->pos.y - e1->pos.y;
        double distance = fmax(sqrt(dx * dx + dy * dy), 0.01);

        double dvx = e1->v.x - e2->v.x;
        double dvy = e1->v.y - e2->v.y;

        return dvx * (dx / distance) + dvy * (dy / distance);
}

static void entity_handle_collision(World *world, Entity *e1, Entity *e2,
                                    const CollisionManifold *manifold) {
        Vec2 rel_v = {e2->v.x - e1->v.x, e2->v.y - e1->v.y};

        double vel_along_normal =
            rel_v.x * manifold->normal.x + rel_v.y * manifold->normal.y;

        if (vel_along_normal > 0) {
                return; // Entities are moving away from eachother
        }

        if (e1->definition->collide)
                e1->definition->collide(world, e1, e2);
        if (e2->definition->collide)
                e2->definition->collide(world, e2, e1);

        if (e1->pinned && e2->pinned)
                return; // Both entities are pinned, do not move them

        double e =
            (e1->definition->restitution + e2->definition->restitution) / 2.0;

        double inv_m1 = e1->pinned ? 0.0
                                   : (e1->definition->mass > 0.0
                                          ? 1.0 / e1->definition->mass
                                          : 0.0);
        double inv_m2 = e2->pinned ? 0.0
                                   : (e2->definition->mass > 0.0
                                          ? 1.0 / e2->definition->mass
                                          : 0.0);

        double j = -(1.0 + e) * vel_along_normal / (inv_m1 + inv_m2);

        e1->v.x -= j * inv_m1 * manifold->normal.x;
        e1->v.y -= j * inv_m1 * manifold->normal.y;
        e2->v.x += j * inv_m2 * manifold->normal.x;
        e2->v.y += j * inv_m2 * manifold->normal.y;
}

void entity_physics_step(Entity *entity, World *world) {
        if (entity->definition->mass <= 0.0 || entity->pinned)
                return; // Object is immovable

        // Detect and handle collisions
        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *a = &world->entities[i];
                if (!a->active)
                        continue;

                for (int j = i + 1; j < MAX_ENTITIES; j++) {
                        Entity *b = &world->entities[j];
                        if (!b->active)
                                continue;

                        CollisionManifold manifold;
                        if (entity_is_colliding(a, b, &manifold)) {
                                entity_fix_overlap(a, b, &manifold);
                                entity_handle_collision(world, a, b, &manifold);
                        }
                }
        }

        // If not in space mode, apply world gravity
        if (!world->space_mode) {
                entity->a.x += world->gravity.x;
                entity->a.y += world->gravity.y;
        }

        // Calculate gravitational field stregth of all other entities
        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *other = &world->entities[i];
                if (entity == other || !other->active)
                        continue;

                Vec2 a = entity_gravitational_field_strength(entity, other);
                entity->a.x += a.x;
                entity->a.y += a.y;
        }
}

void entity_physics_integrate(Entity *entity, double *dt) {
        entity->v.x += entity->a.x * *dt;
        entity->v.y += entity->a.y * *dt;

        entity->pos.x += entity->v.x * *dt;
        entity->pos.y += entity->v.y * *dt;

        entity->omega += (entity->torque * entity->inv_inertia) * *dt;
        entity->angle += entity->omega * *dt;

        // Damping
        entity->omega *= 0.99;
        entity->torque = 0.0;
}

void entity_constrain_to_bounds(Entity *entity, World *world) {
        if (!entity->active || entity->definition->mass <= 0.0)
                return;

        double floor_y = (double)world->height;
        Vec2 contact_point;
        double penetration;

        if (get_lowest_point(entity, &contact_point, &penetration, floor_y)) {
                entity->pos.y -= penetration;

                Vec2 r = {contact_point.x - entity->pos.x,
                          contact_point.y - entity->pos.y};

                double v_contact_y = entity->v.y + entity->omega * r.x;

                if (v_contact_y > 0.0) {
                        double restitution = entity->definition->restitution;

                        double inv_m = 1.0 / entity->definition->mass;
                        double inv_mass_eff =
                            inv_m + (r.x * r.x) * entity->inv_inertia;

                        // Magnitude of upward impulse
                        double j =
                            (1.0 + restitution) * v_contact_y / inv_mass_eff;

                        entity->v.y -= j * inv_m;

                        entity->omega -= (j * r.x) * entity->inv_inertia;

                        // To prevent jittering, apply a threshold to velocity
                        if (fabs(entity->v.y) < 15.0) {
                                entity->v.y = 0.0;
                        }
                        if (fabs(entity->omega) < 0.2) {
                                entity->omega = 0.0;
                        }

                        // Ground friction
                        entity->v.x *= 0.96; // TODO: Make this a constant
                        entity->omega *= 0.96;
                }

                // Ceiling and side walls
                double radius = entity->definition->radius * entity->scale;
                if (entity->pos.x - radius < 0) {
                        entity->pos.x = radius;
                        entity->v.x =
                            -entity->v.x * entity->definition->restitution;
                }
                if (entity->pos.x + radius > world->width) {
                        entity->pos.x = world->width - radius;
                        entity->v.x =
                            -entity->v.x * entity->definition->restitution;
                }
                if (entity->pos.y - radius < 0) {
                        entity->pos.y = radius;
                        entity->v.y =
                            -entity->v.y * entity->definition->restitution;
                }
        }

        double radius = entity->definition->radius * entity->scale;
        double restitution = entity->definition->restitution;

        if (entity->pos.y + radius > world->height) {
                entity->pos.y = world->height - radius;

                // To prevent jittering
                if (fabs(entity->v.y) < 25.0) {
                        entity->v.y = 0.0;
                } else {
                        entity->v.y = -entity->v.y * restitution;
                }

                entity->v.x *= 0.96; // Ground friction
        }

        if (entity->pos.y - radius < 0) {
                entity->pos.y = radius;
                entity->v.y = -entity->v.y * restitution;
        }

        if (entity->pos.x + radius > world->width) {
                entity->pos.x = world->width - radius;
                entity->v.x = -entity->v.x * restitution;
        }

        if (entity->pos.x - radius < 0) {
                entity->pos.x = radius;
                entity->v.x = -entity->v.x * restitution;
        }
}
