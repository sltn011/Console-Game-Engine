#pragma once

#include "Vec3d.hpp"
#include <array>

namespace GE {

    class Matrix4x4 {

        class Row {
            
            std::array<float, 4> m_vals;

        public:

            float &operator[](int index);

            float operator[](int index) const;

        };

        std::array<Row, 4> m_rows;

    public:

        Row &operator[](int index);

        Row const &operator[](int index) const;

        Vec3D multiplyVector(Vec3D const &v) const;

        Matrix4x4 multiplyMatrix(Matrix4x4 const &v) const;

        static Matrix4x4 getIdentity();

        static Matrix4x4 makeRotationX(float angleRad);

        static Matrix4x4 makeRotationY(float angleRad);

        static Matrix4x4 makeRotationZ(float angleRad);

        static Matrix4x4 makeTranslation(float x, float y, float z);

        static Matrix4x4 makeProjection(float fovDegrees, float aspectRatio, float zNear, float zFar);

    };

} // GE


