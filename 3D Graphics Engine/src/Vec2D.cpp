#include "Vec2D.hpp"

namespace GE {

    Vec2D &Vec2D::operator+=(Vec2D const &rhs) {
        m_u += rhs.m_u;
        m_v += rhs.m_v;
        return *this;
    }

    Vec2D &Vec2D::operator-=(Vec2D const &rhs) {
        m_u -= rhs.m_u;
        m_v -= rhs.m_v;
        return *this;
    }

    Vec2D &Vec2D::operator*=(float val) {
        m_u *= val;
        m_v *= val;
        return *this;
    }

    Vec2D &Vec2D::operator/=(float val) {
        m_u /= val;
        m_v /= val;
        return *this;
    }

    float Vec2D::dotProduct(Vec2D const &rhs) const {
        return m_u * rhs.m_u + m_v * rhs.m_v;
    }

    float Vec2D::length() const {
        return std::sqrtf(dotProduct(*this));
    }

    Vec2D Vec2D::getNormalized() const {
        float l = length();
        return Vec2D{ m_u / l, m_v / l };
    }

    Vec2D &Vec2D::normalize() {
        *this = getNormalized();
        return *this;
    }

    Vec2D operator+(Vec2D lhs, Vec2D const &rhs) {
        lhs += rhs;
        return lhs;
    }

    Vec2D operator-(Vec2D lhs, Vec2D const &rhs) {
        lhs -= rhs;
        return lhs;
    }

    Vec2D operator*(Vec2D lhs, float val) {
        lhs *= val;
        return lhs;
    }

    Vec2D operator*(float val, Vec2D rhs) {
        rhs *= val;
        return rhs;
    }

    Vec2D operator/(Vec2D lhs, float val) {
        lhs /= val;
        return lhs;
    }

} // GE
