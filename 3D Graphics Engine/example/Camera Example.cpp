#include "Console Game Engine.hpp"
#include "Vec3D.hpp"
#include "Triangle.hpp"
#include "Matrix4x4.hpp"
#include "Mesh.hpp"
#include <algorithm>
#include <queue>

class Graphics3DEngine : public CGE::BaseGameEngine {
public:
    Graphics3DEngine() {
        m_appName = L"3D Graphics";
    }

    bool userCreate() override {

        if (!m_mesh.loadFromFile("3D Models/mountains.obj")) {
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

        // Controls
        if (getKey(VK_SPACE).isHeld) {
            m_camera.m_y += m_cameraMoveSpeed * elapsedTime;
        }
        if (getKey(VK_LSHIFT).isHeld) {
            m_camera.m_y -= m_cameraMoveSpeed * elapsedTime;
        }
        if (getKey(L'W').isHeld) {
            GE::Vec3D forwardVec = m_lookDir * (m_cameraMoveSpeed * elapsedTime);
            m_camera += forwardVec;
        }
        if (getKey(L'S').isHeld) {
            GE::Vec3D forwardVec = m_lookDir * (m_cameraMoveSpeed * elapsedTime);
            m_camera -= forwardVec;
        }
        if (getKey(L'A').isHeld) {
            // We are adding to angle because in Cartesian coordinate system
            // positive angle is rotating point counter clockwise
            m_yawAngle += m_cameraRotationSpeed * elapsedTime;
        }
        if (getKey(L'D').isHeld) {
            // We are substracting to angle because in Cartesian coordinate system
            // negative angle is rotating point counter clockwise
            m_yawAngle -= m_cameraRotationSpeed * elapsedTime;
        }

        // Clear screen
        fill(0, 0, m_screenWidth, m_screenHeight, CGE::Pixel::Solid, CGE::Color::FG_Black);

        // Vector of objects projected to 2D that will be rendered
        std::vector<GE::Triangle> trianglesToDraw;

        // Draw triangles
        for (auto const &triangle : m_mesh.m_triangles) {

            // Our up is same as Y axis
            GE::Vec3D upVec(0.0f, 1.0f, 0.0f);

            // Default direction camera is 
            GE::Vec3D targetVec(0.0f, 0.0f, 1.0f);

            // Matrix that rotates our target vector around Y axis (so camera can look left and right)
            GE::Matrix4x4 cameraRotation = GE::Matrix4x4::makeRotationY(m_yawAngle);

            // Rotate target vector to where camera is looking (vector is centered at 0,0,0)
            m_lookDir = cameraRotation.multiplyVector(targetVec);

            // Translate target vector to camera location
            targetVec = m_camera + m_lookDir;

            // Creating pointAt matrix
            GE::Matrix4x4 matrixPointAt = pointAtMatrix(m_camera, targetVec, upVec);

            // To make it behave like a camera we find inverse matrix
            // We dont actually rotate camera in the world
            // we rotate world around camera
            // So if camera looks 30 degrees to the right
            // we actually move world around camera 30 degrees to the left
            GE::Matrix4x4 viewMat = quickInverseMatrix(matrixPointAt);

            // Two sides of a triangle
            GE::Vec3D line1 = triangle.m_vertices[1] - triangle.m_vertices[0];
            GE::Vec3D line2 = triangle.m_vertices[2] - triangle.m_vertices[0];

            // Normal vector for our triangle
            GE::Vec3D normal = line1.crossProduct(line2);

            // Normalizing normal vector
            normal.normalize();

            // Calculating dot product between normal and vector from camera to point of a triangle
            // We can choose any point of a triangle bcz they all lie in a plane
            float dotProduct = normal.dotProduct(triangle.m_vertices[0] - m_camera);

            // We can only see the side of a cube if dot product < 0
            if (dotProduct < 0.0f) {

                // Illumination (normalized vector)
                GE::Vec3D lightDirection = { 0.0f, 1.0f, -1.0f };
                lightDirection.normalize();

                // Dot product between normal and light direction vectors
                float dp = max(0.1f, normal.dotProduct(lightDirection));

                // Getting color of a cube pixel and pixel type using illumination power
                CHAR_INFO c = getColor(dp);

                // Convert world space -> view space
                GE::Triangle viewedTriang{};
                for (int i = 0; i < 3; ++i) {
                    viewedTriang.m_vertices[i] = viewMat.multiplyVector(triangle.m_vertices[i]);
                }
                viewedTriang.m_color = c.Attributes;
                viewedTriang.m_pixel = c.Char.UnicodeChar;

                // Clip viewed triangle - it can create 1 or 2 triangles(or none)
                // This part clips triangles that are in front of the camera eyes very very close
                // We need to do this before projecting triangles to 2D because we will divide coords by Z value
                // which can be close to 0. Dividing by 0 can cause many visual bugs
                int numTriangs = 0;
                GE::Triangle clipped[2];
                numTriangs = clipTriangleAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, viewedTriang, clipped[0], clipped[1]);

                for (int t = 0; t < numTriangs; ++t) {

                    // Project triangle from 3D to 2D
                    GE::Triangle projectedTriang{};
                    for (int i = 0; i < 3; ++i) {
                        projectedTriang.m_vertices[i] = m_projectionMatrix.multiplyVector(clipped[t].m_vertices[i]);
                        projectedTriang.m_vertices[i] /= projectedTriang.m_vertices[i].m_w;
                    }
                    projectedTriang.m_color = clipped[t].m_color;
                    projectedTriang.m_pixel = clipped[t].m_pixel;

                    // Y inverted so put it back
                    for (int i = 0; i < 3; ++i) {
                        projectedTriang.m_vertices[i].m_y *= -1.0f;
                    }

                    // Scale into view
                    for (int i = 0; i < 3; ++i) {

                        // Shift normalized coordinates from [-1; +1] to [0; +2] 
                        GE::Vec3D offsetView = { 1.0f, 1.0f, 0.0f };
                        projectedTriang.m_vertices[i] += offsetView;

                        // Divide coordinates to become [0; +1] and muliply them by screen dimensions 
                        projectedTriang.m_vertices[i].m_x *= 0.5f * (float)m_screenWidth;
                        projectedTriang.m_vertices[i].m_y *= 0.5f * (float)m_screenHeight;
                    }

                    // Add triangle to vector of objects that will be rendered
                    trianglesToDraw.push_back(projectedTriang);
                }
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

        for (auto const &triToRaster : trianglesToDraw) {

            // Clip triangle against all four screen edges
            // Store created triangles in a list so we wont test same triangle twice

            std::queue<GE::Triangle> queueTriangles;
            GE::Triangle clipped[2];

            // Add initial triangle
            queueTriangles.push(triToRaster);
            size_t numNewTriangs = 1;

            // For each plane
            for (int p = 0; p < 4; ++p) {
                int numTriangsToAdd = 0;
                while (numNewTriangs > 0) {
                    // Take triangle from queue
                    GE::Triangle test = queueTriangles.front();
                    queueTriangles.pop();
                    --numNewTriangs;

                    // Clip triangle
                    switch (p) {
                    case 0: numTriangsToAdd = clipTriangleAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 1: numTriangsToAdd = clipTriangleAgainstPlane({ 0.0f, (float)m_screenHeight - 1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 2: numTriangsToAdd = clipTriangleAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    case 3: numTriangsToAdd = clipTriangleAgainstPlane({ (float)m_screenWidth - 1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
                    }

                    // Add new triangles to queue
                    // These triangles dont go out from the current plane
                    for (int i = 0; i < numTriangsToAdd; ++i) {
                        queueTriangles.push(clipped[i]);
                    }
                }
                numNewTriangs = queueTriangles.size();
            }

            while (!queueTriangles.empty()) {
                GE::Triangle &t = queueTriangles.front();
                queueTriangles.pop();
                fillTriangle(
                    (short)t.m_vertices[0].m_x, (short)t.m_vertices[0].m_y,
                    (short)t.m_vertices[1].m_x, (short)t.m_vertices[1].m_y,
                    (short)t.m_vertices[2].m_x, (short)t.m_vertices[2].m_y,
                    t.m_pixel, t.m_color
                );

                // Uncomment for debugging - will draw triangles outlines
                /*
                drawTriangle(
                    (short)t.m_vertices[0].m_x, (short)t.m_vertices[0].m_y,
                    (short)t.m_vertices[1].m_x, (short)t.m_vertices[1].m_y,
                    (short)t.m_vertices[2].m_x, (short)t.m_vertices[2].m_y,
                    CGE::Pixel::Solid, CGE::Color::FG_Black
                );
                */
            }
        }

        return true;
    }

    // Create 4x4 Matrix 
    // @pos - where the object should be
    // @target - object's forward vec
    // @up - objects vector that shows where up is
    GE::Matrix4x4 pointAtMatrix(GE::Vec3D const &pos, GE::Vec3D const &target, GE::Vec3D const &up) {
        // New Forward direction - for camera its the direction camera looks
        GE::Vec3D newForward = (target - pos).normalize();

        // New Up direction
        GE::Vec3D r = newForward * up.dotProduct(newForward);
        GE::Vec3D newUp = (up - r).normalize();

        // New Right direction
        GE::Vec3D newRight = newUp.crossProduct(newForward);

        // Construct dimensioning and translationing matrix
        GE::Matrix4x4 res{};
        res[0][0] = newRight.m_x;      res[0][1] = newRight.m_y;      res[0][2] = newRight.m_z;      res[0][3] = 0;
        res[1][0] = newUp.m_x;         res[1][1] = newUp.m_y;         res[1][2] = newUp.m_z;         res[1][3] = 0;
        res[2][0] = newForward.m_x;    res[2][1] = newForward.m_y;    res[2][2] = newForward.m_z;    res[2][3] = 0;
        res[3][0] = pos.m_x;           res[3][1] = pos.m_y;           res[3][2] = pos.m_z;           res[3][3] = 1;
        return res;
    }

    // Only for pointAt matrix -> lookAt matrix and only for rotation/translation
    GE::Matrix4x4 quickInverseMatrix(GE::Matrix4x4 const &matrix) {
        GE::Matrix4x4 res{};
        res[0][0] = matrix[0][0];    res[0][1] = matrix[1][0];    res[0][2] = matrix[2][0];    res[0][3] = 0.0f;
        res[1][0] = matrix[0][1];    res[1][1] = matrix[1][1];    res[1][2] = matrix[2][1];    res[1][3] = 0.0f;
        res[2][0] = matrix[0][2];    res[2][1] = matrix[1][2];    res[2][2] = matrix[2][2];    res[2][3] = 0.0f;
        res[3][0] = -(matrix[3][0] * res[0][0] + matrix[3][1] * res[1][0] + matrix[3][2] * res[2][0]);
        res[3][1] = -(matrix[3][0] * res[0][1] + matrix[3][1] * res[1][1] + matrix[3][2] * res[2][1]);
        res[3][2] = -(matrix[3][0] * res[0][2] + matrix[3][1] * res[1][2] + matrix[3][2] * res[2][2]);
        res[3][3] = 1.0f;
        return res;
    }

    int clipTriangleAgainstPlane(GE::Vec3D planePoint, GE::Vec3D planeNorm, GE::Triangle const &in, GE::Triangle &out1, GE::Triangle &out2) {
        planeNorm.normalize();

        // Return signed shortest distance from point to plane, plane normal must be normalized
        auto dist = [&](GE::Vec3D const &p) {
            GE::Vec3D n = p.getNormalized();
            return (planeNorm.m_x * p.m_x + planeNorm.m_y * p.m_y + planeNorm.m_z * p.m_z - planeNorm.dotProduct(planePoint));
        };

        // Arrays to classify points as inside plane and outside
        GE::Vec3D const *insidePoints[3]; int numInsidePoints = 0;
        GE::Vec3D const *outsidePoints[3]; int numOutsidePoints = 0;

        // Get signed distance from each triangle point to plane
        float ds[3];
        for (int i = 0; i < 3; ++i) {
            ds[i] = dist(in.m_vertices[i]);
        }

        // Classify points by distance to plane
        for (int i = 0; i < 3; ++i) {
            if (ds[i] >= 0) {
                insidePoints[numInsidePoints++] = &in.m_vertices[i];
            }
            else {
                outsidePoints[numOutsidePoints++] = &in.m_vertices[i];
            }
        }

        // Four possible outcomes
        if (numInsidePoints == 0) {
            // Triangle not inside plane - we discard it
            
            // No returned triangles valid
            return 0;
        }
        else if (numInsidePoints == 3) {
            // Triangle inside plane - we keep it
            out1 = in;

            // Just one returned triangle valid
            return 1;
        }
        else if (numInsidePoints == 1 && numOutsidePoints == 2) {
            // We cut off part of a triangle outside plane
            // and create one new triangle

            // Copy appearance info to new triangle
            out1.m_color = in.m_color;
            out1.m_pixel = in.m_pixel;

            // Inside point is valid - keep it
            out1.m_vertices[0] = *insidePoints[0];

            // Other two point are points where input triangle lines intersect with plane
            out1.m_vertices[1] = GE::Vec3D::intersectPlane(planePoint, planeNorm, *insidePoints[0], *outsidePoints[0]);
            out1.m_vertices[2] = GE::Vec3D::intersectPlane(planePoint, planeNorm, *insidePoints[0], *outsidePoints[1]);

            // Returning newly formed triangle
            return 1;
        }
        else if (numInsidePoints == 2 && numOutsidePoints == 1) {
            // After clipping this triangle 2 points will remain
            // and 2 additional point will be created by intersection with plane
            // Bcz of that we will have to break resulting quad into two triangles

            // Copy appearance
            out1.m_color = in.m_color;
            out1.m_pixel = in.m_pixel;

            out2.m_color = in.m_color;
            out2.m_pixel = in.m_pixel;

            // First triangle consists of two inside points and one new point of intersection
            out1.m_vertices[0] = *insidePoints[0];
            out1.m_vertices[1] = *insidePoints[1];
            out1.m_vertices[2] = GE::Vec3D::intersectPlane(planePoint, planeNorm, *insidePoints[0], *outsidePoints[0]);

            // Second triangle consists of one inside point, point of intersection and point created above
            out2.m_vertices[0] = *insidePoints[1];
            out2.m_vertices[1] = out1.m_vertices[2];
            out2.m_vertices[2] = GE::Vec3D::intersectPlane(planePoint, planeNorm, *insidePoints[1], *outsidePoints[0]);

            // Return 2 new triangles
            return 2;
        }
        // In case some error happened - drop results
        return 0;
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
    GE::Vec3D m_lookDir{};

    // Variable to rotate model
    float m_theta = 0.0f;

    // Rotation around Y axis
    float m_yawAngle = 0.0f;

    float m_cameraMoveSpeed = 8.0f;
    float m_cameraRotationSpeed = m_cameraMoveSpeed / 2.0f;
};

int main() {
    Graphics3DEngine engine;
    if (engine.createConsole(250, 200, 4, 4)) {
        engine.start();
    }
}
