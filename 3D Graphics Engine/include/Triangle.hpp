#pragma once

#include "Vec3D.hpp"
#include "Vec2D.hpp"

namespace GE {

    struct Triangle {
        Vec3D m_vertices[3];
        short m_color;
        wchar_t m_pixel;
    };

    struct TexturedTriangle {
        Vec3D m_vertices[3];
        Vec2D m_textures[3];
        short m_color;
        wchar_t m_pixel;
    };

} // GE

