#pragma once

#include <cmath>

namespace GE {

    struct Vec2D {
        float m_u;
        float m_v;
        float m_w = 1.0f; // Need for sensible matrix vector multiplication

        Vec2D &operator+=(Vec2D const &rhs);

        Vec2D &operator-=(Vec2D const &rhs);

        Vec2D &operator*=(float val);

        Vec2D &operator/=(float val);

        float dotProduct(Vec2D const &rhs) const;

        float length() const;

        Vec2D getNormalized() const;

        Vec2D &normalize();
    };

    Vec2D operator+(Vec2D lhs, Vec2D const &rhs);

    Vec2D operator-(Vec2D lhs, Vec2D const &rhs);

    Vec2D operator*(Vec2D lhs, float val);

    Vec2D operator*(float val, Vec2D rhs);

    Vec2D operator/(Vec2D lhs, float val);

} // GE