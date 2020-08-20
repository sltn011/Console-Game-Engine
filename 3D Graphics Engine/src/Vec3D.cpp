#include "Vec3D.hpp"

namespace GE {

    Vec3D::Vec3D() : m_x{ 0.0f }, m_y{ 0.0f }, m_z{ 0.0f } {}

    Vec3D::Vec3D(float x, float y, float z) : m_x{ x }, m_y{ y }, m_z{ z } {}

    Vec3D & Vec3D::operator+=(Vec3D const &rhs) {
        m_x += rhs.m_x;
        m_y += rhs.m_y;
        m_z += rhs.m_z;
        return *this;
    }

    Vec3D & Vec3D::operator-=(Vec3D const &rhs) {
        m_x -= rhs.m_x;
        m_y -= rhs.m_y;
        m_z -= rhs.m_z;
        return *this;
    }

    Vec3D & Vec3D::operator*=(float val) {
        m_x *= val;
        m_y *= val;
        m_z *= val;
        return *this;
    }

    Vec3D & Vec3D::operator/=(float val) {
        m_x /= val;
        m_y /= val;
        m_z /= val;
        return *this;
    }

    float Vec3D::dotProduct(Vec3D const &rhs) const {
        return m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
    }

    Vec3D Vec3D::crossProduct(Vec3D const &rhs) const {
        float cx = m_y * rhs.m_z - m_z * rhs.m_y;
        float cy = m_z * rhs.m_x - m_x * rhs.m_z;
        float cz = m_x * rhs.m_y - m_y * rhs.m_x;
        return Vec3D(cx, cy, cz);
    }

    float Vec3D::length() const {
        return std::sqrtf(dotProduct(*this));
    }

    Vec3D Vec3D::getNormalized() const {
        float l = length();
        return Vec3D(m_x / l, m_y / l, m_z / l);
    }

    void Vec3D::normalize() {
        *this = getNormalized();
    }

    Vec3D operator+(Vec3D lhs, Vec3D const &rhs) {
        lhs += rhs;
        return lhs;
    }

    Vec3D operator-(Vec3D lhs, Vec3D const &rhs) {
        lhs -= rhs;
        return lhs;
    }

    Vec3D operator*(Vec3D lhs, float val) {
        lhs *= val;
        return lhs;
    }

    Vec3D operator/(Vec3D lhs, float val) {
        lhs /= val;
        return lhs;
    }

} // GE
