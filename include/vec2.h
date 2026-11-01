#pragma once

#include <math.h>

typedef struct {
        double x, y;
} Vec2;

static inline Vec2 vec2_add(Vec2 a, Vec2 b) {
        return (Vec2){a.x + b.x, a.y + b.y};
}

static inline Vec2 vec2_sub(Vec2 a, Vec2 b) {
        return (Vec2){a.x - b.x, a.y - b.y};
}

static inline Vec2 vec2_scale(Vec2 v, double s) {
        return (Vec2){v.x * s, v.y * s};
}

static inline double vec2_dot(Vec2 a, Vec2 b) {
        return a.x * b.x + a.y * b.y;
}

static inline double vec2_cross(Vec2 a, Vec2 b) {
        return a.x * b.y - a.y * b.x;
}

static inline double vec2_length_squared(Vec2 v) {
        return v.x * v.x + v.y * v.y;
}

static inline double vec2_length(Vec2 v) {
        return sqrt(vec2_length_squared(v));
}

static inline Vec2 vec2_normalize(Vec2 v) {
        double len = vec2_length(v);
        if (len > 0.00001) {
                return vec2_scale(v, 1.0 / len);
        }
        return (Vec2){0.0, 0.0};
}

static inline double vec2_distance_squared(Vec2 a, Vec2 b) {
        return vec2_length_squared(vec2_sub(a, b));
}

static inline double vec2_distance(Vec2 a, Vec2 b) {
        return vec2_length(vec2_sub(a, b));
}

static inline Vec2 vec2_rotate(Vec2 v, double angle) {
        double cos_a = cos(angle);
        double sin_a = sin(angle);
        return (Vec2){v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a};
}