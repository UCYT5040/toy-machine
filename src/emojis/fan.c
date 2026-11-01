#include <math.h>

#include "world.h"

void emoji_fan_update(World *world, Entity *self, double dt) {
        Entity *in_stream[MAX_ENTITIES];
        int in_stream_count = 0;

        double s = sin(self->angle), c = cos(self->angle);
        if (fabs(s) < 1e-9) /* Horizontal strip */ {
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *other = &world->entities[i];

                        double reach = self->definition->radius +
                                       other->definition->radius;
                        if (other->pos.y >= self->pos.y - reach &&
                            other->pos.y <= self->pos.y + reach) {
                                in_stream[in_stream_count] = other;
                                in_stream_count++;
                        }
                }
        } else if (fabs(c) < 1e-9) /* Vertical strip */ {
                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *other = &world->entities[i];

                        double reach = self->definition->radius +
                                       other->definition->radius;
                        if (other->pos.x >= self->pos.x - reach &&
                            other->pos.x <= self->pos.x + reach) {
                                in_stream[in_stream_count] = other;
                                in_stream_count++;
                        }
                }
        } else /* Strip in any other angle */ {
                double m = tan(self->angle - M_PI_2);
                double b = self->pos.y - m * self->pos.x;

                for (int i = 0; i < MAX_ENTITIES; i++) {
                        Entity *other = &world->entities[i];

                        double m2 = -(1.0 / m);
                        double b2 = other->pos.y - m2 * other->pos.x;

                        double x = (b - b2) / (m2 - m);
                        double y = m * x + b;

                        double dx = other->pos.x - x;
                        double dy = other->pos.y - y;
                        double distance = sqrt(dx * dx + dy * dy);

                        if (distance <= self->definition->radius +
                                            other->definition->radius) {
                                in_stream[in_stream_count] = other;
                                in_stream_count++;
                        }
                }
        }
}