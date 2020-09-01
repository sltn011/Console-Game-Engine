#pragma once

#include "Triangle.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace GE {

    struct Mesh {
        std::vector<Triangle> m_triangles;

        bool loadFromFile(std::string const &path);
    };

    struct TexturedMesh {
        std::vector<TexturedTriangle> m_triangles;

        bool loadFromFile(std::string const &path, bool hasTexture = false);
    };

} // GE



