#pragma once

#include "Vec3d.h"
#include <array>

namespace GE {

    class Matrix4x4 {

        class Column {
            
            std::array<float, 4> m_vals;

        public:

            float &operator[](int index);

            float operator[](int index) const;

        };

        std::array<Column, 4> m_columns;

    public:

        Column &operator[](int index);

        Column const &operator[](int index) const;

        Vec3D multiplyVector(Vec3D const &v) const;

        static Matrix4x4 getIdentity();

        static Matrix4x4 makeRotationX(float angleRad);

        static Matrix4x4 makeRotationY(float angleRad);

        static Matrix4x4 makeRotationZ(float angleRad);

        static Matrix4x4 makeTranslation(float x, float y, float z);

        static Matrix4x4 makeProjection(float fovDegrees, float aspectRatio, float zNear, float zFar);

    };

} // GE


