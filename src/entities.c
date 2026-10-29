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

static void entity_fix_overlap(Entity *self, Entity *other) {
        double dx = other->pos.x - self->pos.x;
        double dy = other->pos.y - self->pos.y;

        double distance = fmax(sqrt(dx * dx + dy * dy), 0.01);

        double diff = self->definition->radius * self->scale +
                      other->definition->radius * other->scale - distance;

        self->pos.x -= dx / distance * diff;
        self->pos.y -= dy / distance * diff;
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

void entity_physics_step(Entity *entity, World *world) {
        if (entity->definition->mass <= 0.0)
                return; // Object is immovable

        // Detect and handle collisions
        for (int i = 0; i < MAX_ENTITIES; i++) {
                Entity *other = &world->entities[i];
                if (entity == other || !other->active)
                        continue;

                if (entity_is_colliding(entity, other)) {
                        entity->v.x *= -1.0;
                        entity->v.y *= -1.0;

                        entity_fix_overlap(entity, other);
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