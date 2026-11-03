#pragma once

#include "collision.h"
#include "emoji_registry.h"
#include "types.h"
#include <stdbool.h>

struct Entity {
        EmojiDefinition *definition;

        bool active;  /* Ignore this entity if false (but do not overwrite!) */
        bool collect; /* Entity slot is free if true */
        int index;
        EmojiType type;

        bool pinned;
        int pinned_target_index;
        int pinned_by_index;

        Vec2 pinned_offset;

        bool processing;        // Only applies to TAG_INVENTORY emojis
        int *processing_inputs; // Only 2 for the factory, but could be more for
                                // other processors
        int processing_input_count;
        double processing_timer;
        const Recipe *current_recipe;
        bool ejecting;
        double eject_timer;

        int needs_coins; // If this is an inventory, and it wants coins, any
                         // adjacent currency inventories will empty into it.

        Vec2 pos;
        Vec2 last_pos;
        Vec2 v;
        Vec2 a;
        Vec2 shake_last_delta;
        double shake_distance;
        double despawn_timer;
        double angle; // In radians
        double omega; // This symbol represents angular velocity
        double torque;
        double inv_inertia; // This is (moment of intertia)^-1

        double scale; // To scale individual entities

        uint64_t tags;
};

static const double G = 6.67430e-11; /* m^3 / (kg · s^2) */

void entity_create_default(Entity *entity, int definition_index, Vec2 pos,
                           int entity_index);

void entity_update_inertia(Entity *entity);

Entity *entity_spawn(World *world, EmojiType type, Vec2 pos);

void entity_remove(World *world, Entity *entity);

// void entity_physics_step(Entity *entity, World *world);
void entity_apply_forces(Entity *entity, World *world);
void world_resolve_collisions(World *world);

void entity_physics_integrate(Entity *entity, double *dt);

void entity_constrain_to_bounds(Entity *entity, World *world);

void entity_handle_collision(World *world, Entity *e1, Entity *e2,
                             const CollisionManifold *manifold);