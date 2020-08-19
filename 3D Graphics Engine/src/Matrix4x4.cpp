#include "Matrix4x4.h"

namespace GE {

    Matrix4x4::Column &Matrix4x4::operator[](int index) {
        return m_columns.at(index);
    }

    Matrix4x4::Column const &Matrix4x4::operator[](int index) const {
        return m_columns.at(index);
    }

    float &Matrix4x4::Column::operator[](int index) {
        return m_vals.at(index);
    }

    float Matrix4x4::Column::operator[](int index) const {
        return m_vals.at(index);
    }

    Vec3D Matrix4x4::multiplyVector(Vec3D const & v) const {
        Vec3D res;
        res.m_x = v.m_x * (*this)[0][0] + v.m_y * (*this)[1][0] + v.m_z * (*this)[2][0] + v.m_w * (*this)[3][0];
        res.m_y = v.m_x * (*this)[0][1] + v.m_y * (*this)[1][1] + v.m_z * (*this)[2][1] + v.m_w * (*this)[3][1];
        res.m_z = v.m_x * (*this)[0][2] + v.m_y * (*this)[1][2] + v.m_z * (*this)[2][2] + v.m_w * (*this)[3][2];
        res.m_w = v.m_x * (*this)[0][3] + v.m_y * (*this)[1][3] + v.m_z * (*this)[2][3] + v.m_w * (*this)[3][3];
        return res;
    }

    Matrix4x4 Matrix4x4::getIdentity() {
        Matrix4x4 m{};
        for (int i = 0; i < 4; ++i) {
            m[i][i] = 1.0f;
        }
        return m;
    }

    Matrix4x4 Matrix4x4::makeRotationX(float angleRad) {
        Matrix4x4 rotX{};
        rotX[0][0] =  1.0f;
        rotX[1][1] =  std::cosf(angleRad);
        rotX[1][2] =  std::sinf(angleRad);
        rotX[2][1] = -std::sinf(angleRad);
        rotX[2][2] =  std::cosf(angleRad);
        rotX[3][3] =  1.0f;
        return rotX;
    }

    Matrix4x4 Matrix4x4::makeRotationY(float angleRad) {
        Matrix4x4 rotY{};
        rotY[0][0] =  std::cosf(angleRad);
        rotY[0][2] =  std::sinf(angleRad);
        rotY[2][0] = -std::sinf(angleRad);
        rotY[1][1] =  1.0f;
        rotY[2][2] =  std::cosf(angleRad);
        rotY[3][3] =  1.0f;
        return rotY;
    }

    Matrix4x4 Matrix4x4::makeRotationZ(float angleRad) {
        Matrix4x4 rotZ{};
        rotZ[0][0] =  std::cosf(angleRad);
        rotZ[0][1] =  std::sinf(angleRad);
        rotZ[1][0] = -std::sinf(angleRad);
        rotZ[1][1] =  std::cosf(angleRad);
        rotZ[2][2] =  1.0f;
        rotZ[3][3] =  1.0f;
        return rotZ;
    }

    Matrix4x4 Matrix4x4::makeTranslation(float x, float y, float z) {
        Matrix4x4 trans = Matrix4x4::getIdentity();
        trans[3][0] = x;
        trans[3][1] = y;
        trans[3][2] = z;
        return trans;
    }

    Matrix4x4 Matrix4x4::makeProjection(float fovDegrees, float aspectRatio, float zNear, float zFar) {
        float fovRad = 1 / std::tanf(fovDegrees * 0.5f / 180.0f * 3.14159f);
        Matrix4x4 proj{};
        proj[0][0] = aspectRatio * fovRad;
        proj[1][1] = fovRad;
        proj[2][2] = zFar / (zFar - zNear);
        proj[3][2] = (-zNear * zFar) / (zFar - zNear);
        proj[2][3] = 1.0f;
        return proj;
    }

} // GE


