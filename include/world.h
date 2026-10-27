#pragma once
#include <SDL2/SDL.h>
#include <stdbool.h>

#define MAX_ENTITIES 1024

typedef struct {
        float x, y;
} Vec2;

typedef enum { EMOJI_NONE = 0, EMOJI_SOCCER_BALL, EMOJI_COUNT } EmojiType;

typedef enum {
        TAG_NONE = 0,
        TAG_FLAMMABLE,
        TAG_CONDUCTIVE,
        TAG_LIVING,
        TAG_METALLIC,
        TAG_HOT,
        TAG_BURNING,
        TAG_WET,
        TAG_ELECTRIC
} EmojiTag;

struct World;
struct Entity;

typedef void (*EmojiUpdateFunction)(struct World *world, struct Entity *self,
                                    float dt);
typedef void (*EmojiCollideFunction)(struct World *world, struct Entity *self,
                                     struct Entity *other);

typedef struct {
        const char *name;
        const char *svg_path;

        int index;

        // TODO: Either remove radius or scale (I accidentally added both)
        float radius; // TODO: Consider allowing custom shapes for non-circular
                      // emojis, or perhaps auto-detecting the shape
        float mass;   // 0.0 means immovable
        float restitution; // Bounciness, from 0.0 (no bounce) to 1.0 (perfectly
                           // elastic)
        float scale; // Relative to a soccer ball (1.0) and planet Earth (2.5)
                     // (consider adding more examples)
        uint32_t default_tags; // TODO: Maybe we don't use a bitmask in case we
                               // add more than 8 tags?

        EmojiUpdateFunction update;
        EmojiCollideFunction collide;
} EmojiDefinition;

typedef struct Entity {
        EmojiDefinition *definition;

        bool active; // TODO: Is this ever false?
        int index;
        EmojiType type;

        Vec2 position;
        Vec2 velocity;
        Vec2 force;
        float angle;
        float scale; // This is in addition to the scale defined in the
                     // EmojiDefinition

        float inverse_mass;
        uint32_t tags; // TODO: See above comment about bitmasking
        float timer;   // Can be used for various purposes
} Entity;

typedef struct World {
        Entity entities[MAX_ENTITIES];
        SDL_Texture *textures[EMOJI_COUNT];

        int width, height;
        float gravity;

        Vec2 mouse_pos;
        bool mouse_down;
        int dragged_entity_index; // -1 if no entity is being dragged

        bool dev_command_mode; // If true, the dev command window is open (game
                               // is paused)
        bool dev_show_hitboxes;
        // TODO: Add more developer/debugging features
} World;

void world_init(World *world, SDL_Renderer *renderer);
void world_update(World *world, float dt);
void world_render(World *world, SDL_Renderer *renderer);
