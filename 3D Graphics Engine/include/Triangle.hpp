#pragma once

#include "Vec3D.hpp"

namespace GE {

    struct Triangle {
        Vec3D m_vertices[3];
        short m_color;
        wchar_t m_pixel;
    };

} // GE

