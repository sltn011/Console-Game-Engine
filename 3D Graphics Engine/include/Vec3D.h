#pragma once

#include <cmath>

namespace GE {

    struct Vec3D {
        float m_x;
        float m_y;
        float m_z;
        float m_w = 1.0f; // Need for sensible matrix vector multiplication

        Vec3D();

        Vec3D(float x, float y, float z);

        Vec3D &operator+=(Vec3D const &rhs);

        Vec3D &operator-=(Vec3D const &rhs);

        Vec3D &operator*=(float val);

        Vec3D &operator/=(float val);

        float dotProduct(Vec3D const &rhs) const;

        Vec3D crossProduct(Vec3D const &rhs) const;

        float length() const;

        Vec3D getNormalized() const;

        void normalize();
    };

    Vec3D operator+(Vec3D lhs, Vec3D const &rhs);

    Vec3D operator-(Vec3D lhs, Vec3D const &rhs);

    Vec3D operator*(Vec3D lhs, float val);

    Vec3D operator/(Vec3D lhs, float val);

} // GE


