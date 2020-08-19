#include "Mesh.h"

namespace GE {

    bool Mesh::loadFromFile(std::string const &path) {

        std::ifstream f(path);
        if (!f.is_open()) {
            return false;
        }

        // Buffer in which lines of file are read
        std::string buf;

        // Local cache of vertices
        std::vector<Vec3D> verts;

        while (std::getline(f, buf)) {
            if (buf.empty()) {
                continue;
            }

            // String stream to parse input from buffer
            std::stringstream line(std::move(buf));

            char lineType;

            line >> lineType;

            switch (lineType) {
            case 'v': {
                Vec3D v;
                line >> v.m_x >> v.m_y >> v.m_z;
                verts.push_back(v);
                break;
            }
            case 'f': {
                int f[3];
                line >> f[0] >> f[1] >> f[2];
                m_triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
                break;
            }
            default: {
                break;
            }
            }
        }
        return true;
    }

} // GE