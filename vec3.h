/**
 * vec3.h
 * By Sebastian Raaphorst, 2023.
 */
#pragma once

#include <cmath>
#include <iostream>

#include "rtweekend.h"

class vec3 final {
private:
    double e[3];

public:
    vec3() noexcept: e{0, 0, 0} {}
    vec3(const double e0, const double e1, const double e2) noexcept: e{e0, e1, e2} {}

    [[nodiscard]] auto x() const noexcept { return e[0]; }
    [[nodiscard]] auto y() const noexcept { return e[1]; }
    [[nodiscard]] auto z() const noexcept { return e[2]; }

    [[nodiscard]] auto operator-() const noexcept{ return vec3(-e[0], -e[1], -e[2]); }
    [[nodiscard]] auto operator[](int i) const { return e[i]; }
    [[nodiscard]] auto &operator[](int i) { return e[i]; }

    [[nodiscard]] inline auto operator+(const vec3 &v) const noexcept {
        return vec3(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]);
    }

    [[nodiscard]] inline auto operator-(const vec3 &v) const noexcept {
        return vec3(e[0] - v.e[0], e[1] - v.e[1], e[2] - v.e[2]);
    }

    [[nodiscard]] inline auto operator*(const vec3 &v) const noexcept {
        return vec3(e[0] * v.e[0], e[1] * v.e[1], e[2] * v.e[2]);
    }

    [[nodiscard]] inline auto operator*(const double t) const noexcept {
        return vec3(e[0] * t, e[1] * t, e[2] * t);
    }

    [[nodiscard]] inline auto operator/(const double t) const {
        return vec3(e[0] / t, e[1] / t, e[2] / t);
    }

    auto &operator+=(const vec3 &v) noexcept {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    auto &operator*=(const double t) noexcept {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    auto &operator/=(const double t) {
        return *this *= 1/t;
    }

    [[nodiscard]] inline auto dot(const vec3 &v) const noexcept {
        return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2];
    }

    [[nodiscard]] inline auto cross(const vec3 &v) const noexcept {
        return vec3(e[1] * v.e[2] - e[2] * v.e[1],
                    e[2] * v.e[0] - e[0] * v.e[2],
                    e[0] * v.e[1] - e[1] * v.e[0]);
    }

    [[nodiscard]] auto length_squared() const noexcept {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    [[nodiscard]] auto length() const noexcept {
        return std::sqrt(length_squared());
    }

    [[nodiscard]] inline auto unit_vector() const {
        return *this / length();
    }

    [[nodiscard]] inline static auto random() noexcept {
        return vec3{random_double(), random_double(), random_double()};
    }

    [[nodiscard]] inline static auto random(double min, double max) noexcept {
        return vec3{random_double(min, max), random_double(min, max), random_double(min, max)};
    }
};

// Type aliases for vec3.
using point3 = vec3;
using color = vec3;

// Utility functions
inline auto &operator<<(std::ostream &out, const vec3 &v) {
    return out << v[0] << ' ' << v[1] << ' ' << v[2];
}

[[nodiscard]] inline auto operator*(const double t, const vec3 &v) {
    return v * t;
}

[[nodiscard]] auto random_in_unit_sphere() noexcept {
    while (true) {
        auto p = vec3::random(-1, 1);
        if (p.length_squared() >= 1)
            continue;
        return p;
    }
}

[[nodiscard]] auto random_unit_vector() noexcept {
    return random_in_unit_sphere().unit_vector();
}

[[nodiscard]] auto random_in_hemisphere(const vec3 &normal) noexcept {
    auto in_unit_sphere = random_in_unit_sphere();
    return in_unit_sphere.dot(normal) > 0.0 ? in_unit_sphere : -in_unit_sphere;
}
