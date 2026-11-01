#pragma once 

#include <stdbool.h>
#include <stdint.h>
#include "vec2.h"

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

typedef struct Recipe
    Recipe; // Forward declaration to prevent circular dependency

typedef void (*EmojiUpdateFunction)(struct World *world, struct Entity *self,
                                    double dt);
typedef void (*EmojiCollideFunction)(struct World *world, struct Entity *self,
                                     struct Entity *other);

