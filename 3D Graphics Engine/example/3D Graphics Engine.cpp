#include "Console Game Engine.hpp"
#include "Vec3D.hpp"
#include "Triangle.hpp"
#include "Matrix4x4.hpp"
#include "Mesh.hpp"
#include <algorithm>

class Graphics3DEngine : public CGE::BaseGameEngine{
public:
    Graphics3DEngine() {
        m_appName = L"3D Graphics";
    }

    bool userCreate() override {

        if (!m_mesh.loadFromFile("3D Models/rocket.obj")) {
            return false;
        }

        float zNear = 0.1f;
        float zFar = 1000.0f;
        float fovDegrees = 90.0f;
        float aspectRatio = (float)m_screenHeight / (float)m_screenWidth;

        // Matrix that projects 3D image to 2D
        m_projectionMatrix = GE::Matrix4x4::makeProjection(fovDegrees, aspectRatio, zNear, zFar);

        return true;
    }

    bool userUpdate(float elapsedTime) override {

        // Clear screen
        fill(0, 0, m_screenWidth, m_screenHeight, CGE::Pixel::Solid, CGE::Color::FG_Black);

        // Variable for rotating points
        m_theta += 1.0f * elapsedTime;

        // Vector of objects projected to 2D that will be rendered
        std::vector<GE::Triangle> trianglesToDraw;

        // Draw triangles
        for (auto const &t : m_mesh.m_triangles) {

            // Matrixes for rotating points around X and Z axis
            GE::Matrix4x4 rotX = GE::Matrix4x4::makeRotationX(m_theta);
            GE::Matrix4x4 rotZ = GE::Matrix4x4::makeRotationZ(m_theta);

            // Rotation of triangle around Z axis
            GE::Triangle rotatedZTriang;
            for (int i = 0; i < 3; ++i) {
                rotatedZTriang.m_vertices[i] = rotZ.multiplyVector(t.m_vertices[i]);
            }

            // Rotation of triangle around X axis
            GE::Triangle rotatedZXTriang;
            for (int i = 0; i < 3; ++i) {
                rotatedZXTriang.m_vertices[i] = rotX.multiplyVector(rotatedZTriang.m_vertices[i]);
            }

            // Offset triangle into screen
            GE::Triangle translatedTriang = rotatedZXTriang;
            for (int i = 0; i < 3; ++i) {
                translatedTriang.m_vertices[i].m_z += 8.0f;
            }

            // Two sides of a triangle
            GE::Vec3D line1 = translatedTriang.m_vertices[1] - translatedTriang.m_vertices[0];
            GE::Vec3D line2 = translatedTriang.m_vertices[2] - translatedTriang.m_vertices[0];

            // Normal vector for our triangle
            GE::Vec3D normal = line1.crossProduct(line2);

            // Normalizing normal vector
            normal.normalize();

            // Calculating dot product between normal and vector from camera to point of a triangle
            // We can choose any point of a triangle bcz they all lie in a plane
            float dotProduct = normal.dotProduct(translatedTriang.m_vertices[0] - m_camera);

            // We can only see the side of a cube if dot product < 0
            if (dotProduct < 0.0f) {

                // Illumination (normalized vector)
                GE::Vec3D lightDirection = { 0.0f, 0.0f, -1.0f };
                lightDirection.normalize();

                // Dot product between normal and light direction vectors
                float dp = normal.dotProduct(lightDirection);

                // Getting color of a cube pixel and pixel type using illumination power
                CHAR_INFO c = getColor(dp);
                translatedTriang.m_color = c.Attributes;
                translatedTriang.m_pixel = c.Char.UnicodeChar;

                // Project triangle from 3D to 2D
                GE::Triangle projectedTriang{};
                for (int i = 0; i < 3; ++i) {
                    projectedTriang.m_vertices[i] = m_projectionMatrix.multiplyVector(translatedTriang.m_vertices[i]);
                    projectedTriang.m_vertices[i] /= projectedTriang.m_vertices[i].m_w;
                }
                projectedTriang.m_color = translatedTriang.m_color;
                projectedTriang.m_pixel = translatedTriang.m_pixel;

                // Scale into view
                for (int i = 0; i < 3; ++i) {

                    // Shift normalized coordinates from [-1; +1] to [0; +2] 
                    projectedTriang.m_vertices[i].m_x += 1.0f;
                    projectedTriang.m_vertices[i].m_y += 1.0f;

                    // Divide coordinates to become [0; +1] and muliply them by screen dimensions 
                    projectedTriang.m_vertices[i].m_x *= 0.5f * (float)m_screenWidth;
                    projectedTriang.m_vertices[i].m_y *= 0.5f * (float)m_screenHeight;
                }

                // Add triangle to vector of objects that will be rendered
                trianglesToDraw.push_back(projectedTriang);

            }
        }

        // Sort triangles to draw from farest to closest
        // Need this to render triangles in correct order
        std::sort(
            trianglesToDraw.begin(),
            trianglesToDraw.end(),
            [](GE::Triangle const &t1, GE::Triangle const &t2) {
                float z1 = (t1.m_vertices[0].m_z + t1.m_vertices[1].m_z + t1.m_vertices[2].m_z) / 3.0f;
                float z2 = (t2.m_vertices[0].m_z + t2.m_vertices[1].m_z + t2.m_vertices[2].m_z) / 3.0f;
                return z1 > z2;
            }
        );

        for (auto const &triangle : trianglesToDraw) {

            fillTriangle(
                (short)triangle.m_vertices[0].m_x, (short)triangle.m_vertices[0].m_y,
                (short)triangle.m_vertices[1].m_x, (short)triangle.m_vertices[1].m_y,
                (short)triangle.m_vertices[2].m_x, (short)triangle.m_vertices[2].m_y,
                triangle.m_pixel,
                triangle.m_color
            );

            //Uncomment for debugging - will show all the triangles
            /*
            drawTriangle(
                (short)triangle.m_vertices[0].m_x, (short)triangle.m_vertices[0].m_y,
                (short)triangle.m_vertices[1].m_x, (short)triangle.m_vertices[1].m_y,
                (short)triangle.m_vertices[2].m_x, (short)triangle.m_vertices[2].m_y,
                CGE::Pixel::Solid,
                CGE::Color::FG_Black
            );
            */
        }

        return true;
    }

    // Get color and pixel type by giving value of light that illuminates point we are shading
    CHAR_INFO getColor(float lum) {
        CGE::baseColorType bgCol, fgCol;
        CGE::basePixelType pix;
        int pixelBw = (int)(13.0f * lum);
        switch (pixelBw) {
        case 0:  bgCol = CGE::Color::BG_Black; fgCol = CGE::Color::FG_Black; pix = CGE::Pixel::Solid; break;

        case 1:  bgCol = CGE::Color::BG_Black; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::Quarter; break;
        case 2:  bgCol = CGE::Color::BG_Black; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::Half; break;
        case 3:  bgCol = CGE::Color::BG_Black; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::ThreeQuarters; break;
        case 4:  bgCol = CGE::Color::BG_Black; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::Solid; break;

        case 5:  bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_Grey; pix = CGE::Pixel::Quarter; break;
        case 6:  bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_Grey; pix = CGE::Pixel::Half; break;
        case 7:  bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_Grey; pix = CGE::Pixel::ThreeQuarters; break;
        case 8:  bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_Grey; pix = CGE::Pixel::Solid; break;

        case 9:  bgCol = CGE::Color::BG_Grey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::Quarter; break;
        case 10: bgCol = CGE::Color::BG_Grey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::Half; break;
        case 11: bgCol = CGE::Color::BG_Grey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::ThreeQuarters; break;
        case 12: bgCol = CGE::Color::BG_Grey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::Solid; break;

        default: bgCol = CGE::Color::BG_Black; fgCol = CGE::Color::FG_Black; pix = CGE::Pixel::Solid; break;
        }

        CHAR_INFO c;
        c.Attributes = bgCol | fgCol;
        c.Char.UnicodeChar = pix;
        return c;
    }

private:

    // Rendered model
    GE::Mesh m_mesh;

    // Matrix that projects 3D image to 2D
    GE::Matrix4x4 m_projectionMatrix;

    // View point
    GE::Vec3D m_camera{};

    // Variable to rotate model
    float m_theta = 0.0f;
};

int main() {
    Graphics3DEngine engine;
    if (engine.createConsole(250, 200, 4, 4)) {
        engine.start();
    }
}