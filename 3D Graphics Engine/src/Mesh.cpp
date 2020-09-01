#include "Mesh.hpp"

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

    bool TexturedMesh::loadFromFile(std::string const &path, bool hasTexture) {

        std::ifstream f(path);
        if (!f.is_open()) {
            return false;
        }

        // Buffer in which lines of file are read
        std::string buf;

        // Local cache of vertices
        std::vector<Vec3D> verts;
        
        // Local cache of texture vertices
        std::vector<Vec2D> texts;

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
                char next;
                line.read(&next, 1);
                if (next == 't') {
                    Vec2D v;
                    line >> v.m_u >> v.m_v;
                    //v.m_u = 1.0f - v.m_u;
                    v.m_v = 1.0f - v.m_v;
                    texts.push_back(v);
                }
                else {
                    Vec3D v;
                    line >> v.m_x >> v.m_y >> v.m_z;
                    verts.push_back(v);
                }
                break;
            }
            case 'f': {
                if (!hasTexture) {
                    int f[3];
                    line >> f[0] >> f[1] >> f[2];
                    m_triangles.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
                    break;
                }
                else {
                    std::string tokens[6];
                    int tokensCount = -1;
                    char c;
                    while (line.read(&c, 1)) {
                        if (c == ' ' || c == '/') {
                            ++tokensCount;
                        }
                        else {
                            tokens[tokensCount].append(1, c);
                        }
                    }

                    m_triangles.push_back({ verts[std::stoi(tokens[0]) - 1], verts[std::stoi(tokens[2]) - 1], verts[std::stoi(tokens[4]) - 1],
                                            texts[std::stoi(tokens[1]) - 1], texts[std::stoi(tokens[3]) - 1], texts[std::stoi(tokens[5]) - 1] });
                    break;
                }
            }
            default: {
                break;
            }
            }
        }
        return true;
    }

} // GE