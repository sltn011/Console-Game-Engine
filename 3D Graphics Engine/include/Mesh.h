#pragma once

#include "Triangle.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace GE {

    struct Mesh {
        std::vector<Triangle> m_triangles;

        bool loadFromFile(std::string const &path);
    };

} // GE



