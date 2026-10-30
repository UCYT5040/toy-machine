#include <math.h>

#include "emoji_registry.h"
#include "entities.h"
#include "world.h"

void entity_create_default(Entity *entity, int definition_index, Vec2 pos,
                           int entity_index) {
        entity->active = true;
        entity->definition = &EMOJI_DEFINITIONS[definition_index];
        entity->type = (EmojiType)definition_index;
        entity->pos = pos;
        entity->last_pos = pos;
        entity->v = (Vec2){0.0, 0.0};
        entity->angle = 0.0;
        entity->scale = 1.0;
        entity->tags = entity->definition->default_tags;
        entity->timer = 0.0;
        entity->index = entity_index;
        entity->a = (Vec2){0.0, 0.0};
}

// Calculates the gravitational field strengh of `other` on `self`
static Vec2 entity_gravitational_field_strength(Entity *self, Entity *other) {
        double dx = other->pos.x - self->pos.x;
        double dy = other->pos.y - self->pos.y;
        double distance = fmax(sqrt(dx * dx + dy * dy), 0.01);

        double a = G * other->definition->mass / (distance * distance);

        return (Vec2){a * dx / distance, a * dy / distance};
}

static bool entity_is_colliding(Entity *e1, Entity *e2) {
        double dx = e2->pos.x - e1->pos.x;
        double dy = e2->pos.y - e1->pos.y;

        // Assuming circular hitboxes for now
        return sqrt(dx * dx + dy * dy) <=
               e1->definition->radius * e1->scale +
                   e2->definition->radius * e2->scale;
}

static void entity_fix_overlap(Entity *e1, Entity *e2) {
        double dx = e2->pos.x - e1->pos.x;
        double dy = e2->pos.y - e1->pos.y;

        double distance = fmax(sqrt(dx * dx + dy * dy), 0.01);

        double diff = e1->definition->radius * e1->scale +
                      e2->definition->radius * e2->scale - distance;

        double nx = dx / distance;
        double ny = dy / distance;

        double total_mass = e1->definition->mass + e2->definition->mass;
        if (total_mass <= 0.0)
                return; // Both are immovable
        double e1share = e2->definition->mass / total_mass;
        double e2share = e1->definition->mass / total_mass;

        e1->pos.x -= nx * diff * e1share;
        e1->pos.y -= ny * diff * e1share;
        e2->pos.x += nx * diff * e2share;
        e2->pos.y += ny * diff * e2share;
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

static void entity_handle_collision(World *world, Entity *e1, Entity *e2) {
        if (entity_relative_velocity(e1, e2) <= 0.0)
                return; /* Entities are moving away from eachother or relative
                           velocity is zero */

        if (e1->definition->collide)
                e1->definition->collide(world, e1, e2);
        if (e2->definition->collide)
                e2->definition->collide(world, e2, e1);

        e1->v = (Vec2){0.0, 0.0};
        e2->v = (Vec2){0.0, 0.0};
}

void entity_physics_step(Entity *entity, World *world) {
        if (entity->definition->mass <= 0.0)
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

                        if (entity_is_colliding(a, b)) {
                                entity_fix_overlap(a, b);
                                entity_handle_collision(world, a, b);
                        }
                }
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
}