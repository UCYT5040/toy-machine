#include <math.h>
#include <stdlib.h>

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
        entity->shake_distance = 0.0;
        entity->despawn_timer = -1.0;
        entity->v = (Vec2){0.0, 0.0};
        entity->angle = 0.0;
        entity->scale = 1.0;
        entity->tags = entity->definition->default_tags;
        entity->index = entity_index;
        entity->omega = 0.0;
        entity->torque = 0.0;
        entity->inv_inertia = 0.0;
        entity->processing = false;
        entity->processing_inputs = NULL;
        entity->processing_input_count = 0;
        entity->current_recipe = NULL;
        entity->ejecting = false;
        entity->eject_timer = 0.0;
        entity->a = (Vec2){0.0, 0.0};
        entity->pinned = false;
        entity->pinned_target_index = -1;
        entity->pinned_by_index = -1;
        entity->pinned_offset = (Vec2){0.0, 0.0};
}

void entity_update_inertia(Entity *entity) {
        double mass = entity->definition->mass;
        if (mass <= 0.0 || entity->pinned)
                entity->inv_inertia = 0.0;
        else {
                double I; // should we rename this? since it's a capital
                          // variable but not a constant
                if (entity->definition->colliders[0].type == COLLIDER_BOX) {
                        double w = entity->definition->colliders[0].box.width *
                                   entity->scale;
                        double h = entity->definition->colliders[0].box.height *
                                   entity->scale;
                        I = (1.0 / 12.0) * mass * (w * w + h * h);
                } else {
                        double r =
                            entity->definition->colliders[0].circle.radius *
                            entity->scale;
                        I = 0.5 * mass * r * r;
                }
                entity->inv_inertia = 1.0 / I;
        }
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
                entity_update_inertia(entity);
        }

        return entity; // No available entity slots
}

void entity_remove(World *world, Entity *entity) {
        if (!world || !entity || !entity->active) {
                return;
        }

        if (world->pressed_entity_index == entity->index) {
                world->pressed_entity_index = -1;
        }

        if (entity->pinned_target_index >= 0 &&
            entity->pinned_target_index < MAX_ENTITIES) {
                Entity *target = &world->entities[entity->pinned_target_index];
                if (target->pinned_by_index == entity->index) {
                        target->pinned = false;
                        target->pinned_by_index = -1;
                }
        }

        if (entity->pinned_by_index >= 0 &&
            entity->pinned_by_index < MAX_ENTITIES) {
                Entity *pin = &world->entities[entity->pinned_by_index];
                if (pin->pinned_target_index == entity->index) {
                        pin->pinned = false;
                        pin->pinned_target_index = -1;
                }
        }

        if (entity->processing_inputs) {
                for (int i = 0; i < entity->processing_input_count; i++) {
                        int input_index = entity->processing_inputs[i];
                        if (input_index >= 0 && input_index < MAX_ENTITIES &&
                            input_index != entity->index) {
                                Entity *input = &world->entities[input_index];
                                if (!input->active) {
                                        input->collect = true;
                                }
                        }
                }

                free(entity->processing_inputs);
                entity->processing_inputs = NULL;
        }

        entity->processing = false;
        entity->processing_input_count = 0;
        entity->processing_timer = 0.0;
        entity->current_recipe = NULL;
        entity->ejecting = false;
        entity->eject_timer = 0.0;

        entity->pinned = false;
        entity->pinned_target_index = -1;
        entity->pinned_by_index = -1;
        entity->pinned_offset = (Vec2){0.0, 0.0};

        entity->v = (Vec2){0.0, 0.0};
        entity->a = (Vec2){0.0, 0.0};
        entity->omega = 0.0;
        entity->torque = 0.0;
        entity->despawn_timer = -1.0;

        entity->active = false;
        entity->collect = true;
}

// Calculates the gravitational field strengh of `other` on `self`
static Vec2 entity_gravitational_field_strength(Entity *self, Entity *other) {
        Vec2 delta = vec2_sub(other->pos, self->pos);
        double distance_sq = vec2_length_squared(delta);
        double distance = fmax(sqrt(distance_sq), 0.01);

        double a = G * other->definition->mass / (distance * distance);

        return vec2_scale(delta, a / distance);
}

void entity_handle_collision(World *world, Entity *e1, Entity *e2,
                             const CollisionManifold *manifold) {
        Entity *contact_first = e1;
        Entity *contact_second = e2;
        if (contact_second->definition->collision_priority >
            contact_first->definition->collision_priority) {
                contact_first = e2;
                contact_second = e1;
        }

        if (contact_first->active &&
            contact_first->definition->collision_priority > 0 &&
            contact_first->definition->collide) {
                contact_first->definition->collide(world, contact_first,
                                                   contact_second);
        }

        if (!e1->active || !e2->active) {
                return;
        }

        if (contact_second->active &&
            contact_second->definition->collision_priority > 0 &&
            contact_second->definition->collide) {
                contact_second->definition->collide(world, contact_second,
                                                    contact_first);
        }

        if (!e1->active || !e2->active) {
                return;
        }

        Vec2 rel_v = vec2_sub(e2->v, e1->v);

        double vel_along_normal = vec2_dot(rel_v, manifold->normal);

        if (vel_along_normal > 0.0) {
                return; // Entities are moving away from eachother
        }

        if (e1->definition->collision_priority == 0 &&
            e1->definition->collide) {
                e1->definition->collide(world, e1, e2);
        }
        if (!e1->active || !e2->active) {
                return;
        }

        if (e2->definition->collision_priority == 0 &&
            e2->definition->collide) {
                e2->definition->collide(world, e2, e1);
        }
        if (!e1->active || !e2->active) {
                return;
        }

        if (e1->pinned && e2->pinned)
                return; // Both entities are pinned, do not move them

        double inv_m1 = e1->pinned ? 0.0
                                   : (e1->definition->mass > 0.0
                                          ? 1.0 / e1->definition->mass
                                          : 0.0);
        double inv_m2 = e2->pinned ? 0.0
                                   : (e2->definition->mass > 0.0
                                          ? 1.0 / e2->definition->mass
                                          : 0.0);

        double inv_I1 = e1->pinned ? 0.0 : e1->inv_inertia;
        double inv_I2 = e2->pinned ? 0.0 : e2->inv_inertia;

        Vec2 r1 = vec2_sub(manifold->contact_point, e1->pos);
        Vec2 r2 = vec2_sub(manifold->contact_point, e2->pos);

        double rn1 = vec2_cross(r1, manifold->normal);
        double rn2 = vec2_cross(r2, manifold->normal);
        double kn =
            inv_m1 + inv_m2 + (rn1 * rn1) * inv_I1 + (rn2 * rn2) * inv_I2;

        if (kn <= 0.0) {
                return;
        }

        double e =
            (e1->definition->restitution + e2->definition->restitution) / 2.0;
        double jn = -(1.0 + e) * vel_along_normal / kn;

        Vec2 tangent = {-manifold->normal.y, manifold->normal.x};
        double vel_along_tangent = vec2_dot(rel_v, tangent);

        double rt1 = vec2_cross(r1, tangent);
        double rt2 = vec2_cross(r2, tangent);
        double kt =
            inv_m1 + inv_m2 + (rt1 * rt1) * inv_I1 + (rt2 * rt2) * inv_I2;

        double jt = 0.0;
        if (kt > 0.0) {
                jt = -vel_along_tangent / kt;
                double mu = 0.35; // Friction coefficient
                double max_jt = mu * jn;
                if (jt > max_jt) {
                        jt = max_jt;
                } else if (jt < -max_jt) {
                        jt = -max_jt;
                }
        }

        Vec2 impulse =
            vec2_add(vec2_scale(manifold->normal, jn), vec2_scale(tangent, jt));

        e1->v = vec2_sub(e1->v, vec2_scale(impulse, inv_m1));
        e1->omega -= vec2_cross(r1, impulse) * inv_I1;

        e2->v = vec2_add(e2->v, vec2_scale(impulse, inv_m2));
        e2->omega += vec2_cross(r2, impulse) * inv_I2;
}

void entity_physics_integrate(Entity *entity, double *dt) {
        entity->v = vec2_add(entity->v, vec2_scale(entity->a, *dt));
        entity->pos = vec2_add(entity->pos, vec2_scale(entity->v, *dt));

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

                Vec2 r = vec2_sub(contact_point, entity->pos);

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
                        double v_contact_x = entity->v.x - entity->omega * r.y;
                        double inv_mass_tangent =
                            inv_m + (r.y * r.y) * entity->inv_inertia;

                        double jt = -v_contact_x / inv_mass_tangent;

                        double mu = 0.45;
                        double max_jt = mu * j;
                        if (jt > max_jt) {
                                jt = max_jt;
                        } else if (jt < -max_jt) {
                                jt = -max_jt;
                        }

                        entity->v.x += jt * inv_m;
                        entity->omega -= (jt * r.y) * entity->inv_inertia;

                        // Little resistance so it comes to a rest eventually
                        entity->v.x *= 0.995;
                        entity->omega *= 0.995;

                        if (entity->definition->wall_collide)
                                entity->definition->wall_collide(
                                    world, entity, (Vec2){0.0, -1.0});
                }
        }

        // Ceiling and side walls
        double radius = entity->definition->radius * entity->scale;
        if (entity->pos.x - radius < 0) {
                entity->pos.x = radius;
                entity->v.x = -entity->v.x * entity->definition->restitution;

                if (entity->definition->wall_collide)
                        entity->definition->wall_collide(world, entity,
                                                         (Vec2){1.0, 0.0});
        }
        if (entity->pos.x + radius > world->width) {
                entity->pos.x = world->width - radius;
                entity->v.x = -entity->v.x * entity->definition->restitution;

                if (entity->definition->wall_collide)
                        entity->definition->wall_collide(world, entity,
                                                         (Vec2){-1.0, 0.0});
        }
        if (entity->pos.y - radius < 0) {
                entity->pos.y = radius;
                entity->v.y = -entity->v.y * entity->definition->restitution;

                if (entity->definition->wall_collide)
                        entity->definition->wall_collide(world, entity,
                                                         (Vec2){0.0, 1.0});
        }
}

void world_resolve_collisions(World *world) {
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
                                if (!a->active) {
                                        break;
                                }
                        }
                }
        }
}

void entity_apply_forces(Entity *entity, World *world) {
        if (!entity->active || entity->definition->mass <= 0.0 ||
            entity->pinned || entity->tags & TAG_NO_GRAVITY)
                return;

        entity->a = (Vec2){0.0, 0.0};

        // If not in space mode, apply world gravity
        if (!world->space_mode) {
                entity->a = vec2_add(entity->a, world->gravity);
        }

        // Calculate gravitational field stregth of all other entities
        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *other = &world->entities[i];
                if (entity == other || !other->active)
                        continue;

                Vec2 a = entity_gravitational_field_strength(entity, other);
                entity->a = vec2_add(entity->a, a);
        }
}