#include "dev.h"
#include "SDL_render.h"
#include "emoji_registry.h"
#include "entities.h"

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

                                // Find an inactive entity slot
                                for (int j = 0; j < MAX_ENTITIES; j++) {
                                        Entity *entity = &world->entities[j];
                                        if (!entity->active) {
                                                entity_create_default(
                                                    entity, i, (Vec2){x, y}, j);

                                                snprintf(output_buffer,
                                                         output_buffer_size,
                                                         "Spawned %s at (%.2f, "
                                                         "%.2f)",
                                                         emoji_name, x, y);
                                                return;
                                        }
                                }

                                snprintf(output_buffer, output_buffer_size,
                                         "No available slots to spawn new "
                                         "entities.");
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