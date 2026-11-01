#include "dev.h"
#include "SDL_render.h"
#include "emoji_registry.h"
#include "entities.h"
#include "world.h"

#include <SDL2/SDL.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void append_output(char *buffer, size_t buffer_size, size_t *used,
                          const char *format, ...) {
        if (*used >= buffer_size) {
                return;
        }

        va_list args;
        va_start(args, format);
        int written =
            vsnprintf(buffer + *used, buffer_size - *used, format, args);
        va_end(args);

        if (written < 0) {
                return;
        }

        *used += (size_t)written;
        if (*used >= buffer_size) {
                *used = buffer_size - 1;
        }
}

static void dev_command_help(char *output_buffer, size_t output_buffer_size) {
        size_t used = 0;
        output_buffer[0] = '\0';

        append_output(
            output_buffer, output_buffer_size, &used,
            "Commands:\n"
            " help - show this help message\n"
            " spawn <emoji> <x> <y> - spawn an emoji at given coords\n"
            " speed <value> - set simulation speed\n"
            " hitboxes <on/off> - toggle hitboxes\n\n"
            "Emojis: ");

        for (int i = 1; i < EMOJI_COUNT; i++) {
                append_output(output_buffer, output_buffer_size, &used, " %s",
                              EMOJI_DEFINITIONS[i].name);
        }
}

void run_dev_command(World *world, char *command, char *output_buffer,
                     size_t output_buffer_size) {
        char *command_name = strtok(command, " \t\n");
        if (command_name == NULL) {
                snprintf(output_buffer, output_buffer_size,
                         "No command entered.");
                return;
        }
        if (strcmp(command_name, "help") == 0) {
                dev_command_help(output_buffer, output_buffer_size);
        } else if (strcmp(command_name, "spawn") == 0) {
                char *emoji_name = strtok(NULL, " \t\n");
                if (emoji_name == NULL) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Usage: spawn <emoji_name> <x> <y>");
                        return;
                }

                bool found = false;
                for (int i = 1; i < EMOJI_COUNT && !found; i++) {
                        found =
                            strcmp(emoji_name, EMOJI_DEFINITIONS[i].name) == 0;
                        if (found) {
                                char *x_str = strtok(NULL, " \t\n");
                                char *y_str = strtok(NULL, " \t\n");
                                if (x_str == NULL || y_str == NULL) {
                                        snprintf(output_buffer,
                                                 output_buffer_size,
                                                 "Usage: spawn <emoji_name> "
                                                 "<x> <y>");
                                        return;
                                }

                                double x = strtof(x_str, NULL);
                                double y = strtof(y_str, NULL);

                                Entity *entity =
                                    entity_spawn(world, i, (Vec2){x, y});
                                if (entity != NULL) {
                                        snprintf(output_buffer,
                                                 output_buffer_size,
                                                 "Spawned %s at (%.2f, "
                                                 "%.2f)",
                                                 emoji_name, x, y);

                                } else {
                                        snprintf(
                                            output_buffer, output_buffer_size,
                                            "No available slots to spawn new "
                                            "entities.");
                                }
                                return;
                        }
                }

                if (!found) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Emoji not found");
                }
        } else if (strcmp(command_name, "speed") == 0) {
                char *speed_str = strtok(NULL, " \t\n");
                if (speed_str == NULL) {
                        snprintf(
                            output_buffer, output_buffer_size,
                            "Usage: speed <value> (e.g., 0.5 for half speed)");
                        return;
                }

                double speed = strtof(speed_str, NULL);

                // Accept `0.0`, since that can be used to freeze the game
                if (speed < 0.0) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Speed must be non-negative");
                        return;
                }

                world->speed = speed;
                snprintf(output_buffer, output_buffer_size, "Speed set to %.2f",
                         speed);
        } else if (strcmp(command_name, "hitboxes") == 0) {
                char *option_str = strtok(NULL, " \t\n");
                if (option_str == NULL) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Usage: hitboxes <on/off>");
                        return;
                }

                // Skips any whitespace
                while (option_str[0] == ' ')
                        option_str++;

                if (strcmp(option_str, "on") == 0) {
                        world->dev_show_hitboxes = true;
                        snprintf(output_buffer, output_buffer_size,
                                 "Enabled hitboxes");
                } else if (strcmp(option_str, "off") == 0) {
                        world->dev_show_hitboxes = false;
                        snprintf(output_buffer, output_buffer_size,
                                 "Disabled hitboxes");
                } else {
                        snprintf(output_buffer, output_buffer_size,
                                 "Invalid argument to command \"hitboxes\". "
                                 "Only \"on\" and \"off\" are allowed.");
                }
        } else if (strcmp(command_name, "exit") == 0) {
                exit(0);
        } else if (strcmp(command, "velocity") == 0) {
                char *option_str = strtok(NULL, " \t\n");
                if (option_str == NULL) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Usage: velocity <on/off>");
                        return;
                }

                // Skips any whitespace
                while (option_str[0] == ' ')
                        option_str++;

                if (strcmp(option_str, "on") == 0) {
                        world->dev_show_velocity_vectors = true;
                        snprintf(output_buffer, output_buffer_size,
                                 "Enabled velocity vectors");
                } else if (strcmp(option_str, "off") == 0) {
                        world->dev_show_velocity_vectors = false;
                        snprintf(output_buffer, output_buffer_size,
                                 "Disabled velocity vectors");
                } else {
                        snprintf(output_buffer, output_buffer_size,
                                 "Invalid argument to command \"velocity\". "
                                 "Only \"on\" and \"off\" are allowed.");
                }
        } else if (strcmp(command_name, "unlock") == 0) {
                char *emoji_name = strtok(NULL, " \t\n");
                if (emoji_name == NULL) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Usage: unlock <emoji_name>");
                        return;
                }

                bool found = false;
                for (int i = 1; i < EMOJI_COUNT && !found; i++) {
                        found =
                            strcmp(emoji_name, EMOJI_DEFINITIONS[i].name) == 0;
                        if (found) {
                                world->emoji_unlocks[i - 1] = true;

                                snprintf(output_buffer, output_buffer_size,
                                         "Unlocked %s", emoji_name);
                                return;
                        }
                }

                if (!found) {
                        snprintf(output_buffer, output_buffer_size,
                                 "Emoji not found");
                }
        } else if (strcmp(command_name, "unlockall") == 0) {
                for (int i = 1; i < EMOJI_COUNT; i++) {

                        world->emoji_unlocks[i - 1] = true;
                }

                snprintf(output_buffer, output_buffer_size,
                         "Unlocked all emojis");
        } else {
                snprintf(output_buffer, output_buffer_size,
                         "Unknown command %s, type 'help' for\n a list of "
                         "available commands",
                         command_name);
        }
}

void render_velocity_vector(SDL_Renderer *renderer, Entity *entity) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawLine(renderer, entity->pos.x, entity->pos.y,
                           entity->pos.x + entity->v.x,
                           entity->pos.y + entity->v.y);
}

#define CIRCLE_SEGMENTS 32

void render_hitbox(SDL_Renderer *renderer, Entity *entity) {
        for (int i = 0; i < MAX_COLLIDERS; i++) {
                Collider *collider = &entity->definition->colliders[i];

                if (collider->type == COLLIDER_NONE) {
                        continue;
                }

                if (collider->type == COLLIDER_CIRCLE) {
                        double radius = collider->circle.radius;

                        double cx = entity->pos.x + collider->offset.x;
                        double cy = entity->pos.y + collider->offset.y;

                        SDL_Point points[CIRCLE_SEGMENTS + 1];
                        for (int j = 0; j <= CIRCLE_SEGMENTS; j++) {
                                double phi = 2.0 * M_PI * j / CIRCLE_SEGMENTS;
                                points[j].x = (int)(cx + radius * cos(phi));
                                points[j].y = (int)(cy + radius * sin(phi));
                        }

                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                        SDL_RenderDrawLines(renderer, points,
                                            CIRCLE_SEGMENTS + 1);

                        // Draw a line indicating the angle of the collider
                        double angle_line_length = radius;
                        double angle = entity->angle + collider->angle;
                        int x2 = (int)(cx + angle_line_length * cos(angle));
                        int y2 = (int)(cy + angle_line_length * sin(angle));
                        SDL_RenderDrawLine(renderer, (int)cx, (int)cy, x2, y2);

                } else if (collider->type == COLLIDER_BOX) {
                        double hw = collider->box.width / 2.0;
                        double hh = collider->box.height / 2.0;

                        double total_angle = entity->angle + collider->angle;
                        double cos_a = cos(total_angle);
                        double sin_a = sin(total_angle);

                        double cos_e = cos(entity->angle);
                        double sin_e = sin(entity->angle);

                        double cx =
                            entity->pos.x + (collider->offset.x * cos_e -
                                             collider->offset.y * sin_e);
                        double cy =
                            entity->pos.y + (collider->offset.x * sin_e +
                                             collider->offset.y * cos_e);

                        SDL_Point points[5];
                        for (int j = 0; j < 4; j++) {
                                double local_x = (j == 0 || j == 3) ? -hw : hw;
                                double local_y = (j == 0 || j == 1) ? -hh : hh;

                                double rx = local_x * cos_a - local_y * sin_a;
                                double ry = local_x * sin_a + local_y * cos_a;

                                points[j].x = (int)(cx + rx);
                                points[j].y = (int)(cy + ry);
                        }
                        points[4] = points[0]; // Close the box

                        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                        SDL_RenderDrawLines(renderer, points, 5);
                }
        }
}