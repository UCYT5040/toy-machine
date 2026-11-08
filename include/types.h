#pragma once

#include "vec2.h"
#include <stdbool.h>
#include <stdint.h>

#define MAX_ENTITIES 1024

typedef enum { COLLIDER_NONE, COLLIDER_CIRCLE, COLLIDER_BOX } ColliderType;

typedef struct {
        ColliderType type;
        Vec2 offset;
        double angle;
        union {
                struct {
                        double radius;
                } circle;
                struct {
                        double width, height;
                } box;
        };
} Collider;

#define MAX_COLLIDERS 4

typedef struct World World;
typedef struct Entity Entity;
typedef struct Recipe Recipe;

typedef void (*EmojiUpdateFunction)(struct World *world, struct Entity *self,
                                    double dt);
typedef void (*EmojiCollideFunction)(struct World *world, struct Entity *self,
                                     struct Entity *other);

typedef void (*EmojiWallCollideFunction)(struct World *world,
                                         struct Entity *self, Vec2 normal);

typedef void (*EmojiClickFunction)(struct World *world, struct Entity *self);