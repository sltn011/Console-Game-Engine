#include "Console Game Engine.hpp"
#include <vector>

struct Vec3D {
    float x;
    float y;
    float z;
};

struct Triangle {
    Vec3D vertices[3];
};

struct Mesh {
    std::vector<Triangle> triangles;
};

struct Matrix4x4 {
    float m[4][4] = { 0 };
};

class Graphics3DEngine : public CGE::BaseGameEngine{
public:
    Graphics3DEngine() {
        m_appName = L"3D Graphics";
    }

    bool userCreate() override {

        m_meshCube.triangles = {
            
            // South
            {0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f},

            // East
            {1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f},

            // North
            {1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f},

            // West
            {0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f},

            // Top
            {0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f},

            // Bottom
            {1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f},

        };

        float zNear = 0.1f;
        float zFar = 1000.0f;
        float fov = 90.0f;
        float aspectRatio = (float)m_screenHeight / (float)m_screenWidth;
        float fovRad = 1 / std::tanf(fov * 0.5f / 180.0f * 3.14159f);

        // Matrix that projects 3D image to 2D
        m_projectionMatrix.m[0][0] = aspectRatio * fovRad;
        m_projectionMatrix.m[1][1] = fovRad;
        m_projectionMatrix.m[2][2] = zFar / (zFar - zNear);
        m_projectionMatrix.m[3][2] = (-zNear * zFar) / (zFar - zNear);
        m_projectionMatrix.m[2][3] = 1.0f;

        return true;
    }

    bool userUpdate(float elapsedTime) override {

        // Clear screen
        fill(0, 0, m_screenWidth, m_screenHeight, CGE::Pixel::Solid, CGE::Color::FG_Black);

        // Variable for rotating points
        m_theta += 1.0f * elapsedTime;

        // Draw triangles
        for (auto const &t  : m_meshCube.triangles) {

            // Matrixes for rotating points around X and Z axis
            Matrix4x4 rotX;
            Matrix4x4 rotZ;

            rotX.m[0][0] = 1;
            rotX.m[1][1] =  std::cosf(m_theta * 0.5f);
            rotX.m[1][2] =  std::sinf(m_theta * 0.5f);
            rotX.m[2][1] = -std::sinf(m_theta * 0.5f);
            rotX.m[2][2] =  std::cosf(m_theta * 0.5f);
            rotX.m[3][3] = 1;

            rotZ.m[0][0] =  std::cosf(m_theta);
            rotZ.m[0][1] =  std::sinf(m_theta);
            rotZ.m[1][0] = -std::sinf(m_theta);
            rotZ.m[1][1] =  std::cosf(m_theta);
            rotZ.m[2][2] = 1;
            rotZ.m[3][3] = 1;

            // Rotation of triangle around Z axis
            Triangle rotatedZTriang;
            for (int i = 0; i < 3; ++i) {
                multiplyMatrixVector(t.vertices[i], rotatedZTriang.vertices[i], rotZ);
            }

            // Rotation of triangle around X axis
            Triangle rotatedZXTriang;
            for (int i = 0; i < 3; ++i) {
                multiplyMatrixVector(rotatedZTriang.vertices[i], rotatedZXTriang.vertices[i], rotX);
            }

            // Offset triangle into screen
            Triangle translatedTriang = rotatedZXTriang;
            for (int i = 0; i < 3; ++i) {
                translatedTriang.vertices[i].z += 3.0f;
            }

            // Project triangle from 3D to 2D
            Triangle projectedTriang{};
            for (int i = 0; i < 3; ++i) {
                multiplyMatrixVector(translatedTriang.vertices[i], projectedTriang.vertices[i], m_projectionMatrix);
            }

            // Scale into view
            for (int i = 0; i < 3; ++i) {

                // Shift normalized coordinates from [-1; +1] to [0; +2] 
                projectedTriang.vertices[i].x += 1.0f;
                projectedTriang.vertices[i].y += 1.0f;

                // Divide coordinates to become [0; +1] and muliply them by screen dimensions 
                projectedTriang.vertices[i].x *= 0.5f * (float)m_screenWidth;
                projectedTriang.vertices[i].y *= 0.5f * (float)m_screenHeight;
            }

            drawTriangle(
                (short)projectedTriang.vertices[0].x, (short)projectedTriang.vertices[0].y,
                (short)projectedTriang.vertices[1].x, (short)projectedTriang.vertices[1].y,
                (short)projectedTriang.vertices[2].x, (short)projectedTriang.vertices[2].y,
                CGE::Pixel::Solid,
                CGE::Color::FG_White
            );
        }

        return true;
    }

    void multiplyMatrixVector(Vec3D const &in, Vec3D &out, Matrix4x4 const &matr) {
        out.x = in.x * matr.m[0][0] + in.y * matr.m[1][0] + in.z * matr.m[2][0] + matr.m[3][0];
        out.y = in.x * matr.m[0][1] + in.y * matr.m[1][1] + in.z * matr.m[2][1] + matr.m[3][1];
        out.z = in.x * matr.m[0][2] + in.y * matr.m[1][2] + in.z * matr.m[2][2] + matr.m[3][2];
        float coeff = in.x * matr.m[0][3] + in.y * matr.m[1][3] + in.z * matr.m[2][3] + matr.m[3][3];

        if (coeff != 0.0f) {
            out.x /= coeff;
            out.y /= coeff;
            out.z /= coeff;
        }
    }

private:

    Mesh m_meshCube;
    Matrix4x4 m_projectionMatrix;

    float m_theta = 0.0f;
};

int main() {
    Graphics3DEngine engine;
    if (engine.createConsole(200, 200, 4, 4)) {
        engine.start();
    }
}