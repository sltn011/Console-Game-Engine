#pragma once

#include "Vec3D.h"

namespace GE {

    struct Triangle {
        Vec3D m_vertices[3];
        short m_color;
        wchar_t m_pixel;
    };

} // GE

