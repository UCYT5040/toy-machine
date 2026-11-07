#include "dispenser.h"
#include "entities.h"

static double rand_range(double min, double max) {
        return min + (double)rand() / (double)RAND_MAX * (max - min);
}

Entity *dispense_entity(World *world, EmojiType type,
                        const SpewConfig *config) {
        Entity *entity = entity_spawn(world, type, config->pos);

        if (!entity)
                return NULL;

        double angle = config->angle_rad + rand_range(-config->spread_rad / 2.0,
                                                      config->spread_rad / 2.0);
        double speed = rand_range(config->min_speed, config->max_speed);

        entity->v.x = cos(angle) * speed;
        entity->v.y = sin(angle) * speed;
        entity->omega = rand_range(-config->max_spin, config->max_spin);

        return entity;
}

static SpewConfig note_config = {
    .pos = {0, 0},
    .angle_rad = -M_PI / 2.0,
    .spread_rad = M_PI / 4.0,
    .min_speed = 100.0,
    .max_speed = 200.0,
    .max_spin = M_PI,
};

void dispense_note(World *world, const char *text) {
        // Choose left or right side of the screen to dispense the note from
        if (rand() % 2 == 0) {
                note_config.pos.x = 0.0;
                note_config.angle_rad = 0.0;
        } else {
                note_config.pos.x = (double)world->width;
                note_config.angle_rad = M_PI;
        }

        Entity *entity = dispense_entity(world, EMOJI_PAPER, &note_config);

        entity->note_text = text;
}

static SpewConfig currency_config = {
    .pos = {0, 0},
    .angle_rad = -M_PI / 2.0,
    .spread_rad = M_PI / 4.0,
    .min_speed = 150.0,
    .max_speed = 250.0,
    .max_spin = M_PI * 1.5,
};

void dispense_currency(World *world, int amount) {
        // Choose left or right side of the screen to dispense the currency from
        if (rand() % 2 == 0) {
                currency_config.pos.x = 0.0;
                currency_config.angle_rad = 0.0;
        } else {
                currency_config.pos.x = (double)world->width;
                currency_config.angle_rad = M_PI;
        }

        for (int i = 0; i < amount % 10; i++) {
                dispense_entity(world, EMOJI_COIN, &currency_config);
        }

        for (int i = 0; i < amount / 10; i++) {
                dispense_entity(world, EMOJI_COIN, &currency_config);
        }
}
