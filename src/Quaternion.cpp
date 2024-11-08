#include "Quaternion.h"

#include <cmath>

Quaternion::Quaternion() {
    q[0] = q[1] = q[2] = 0.0;
    q[3] = 1.0;
}

Quaternion::Quaternion(const float x, const float y, const float z, const float w) {
    q[0] = x;
    q[1] = y;
    q[2] = z;
    q[3] = w;
}

Quaternion::Quaternion(const float scalar) {
    // copy scalar
    // set first three coords to 0.0
    for (int i = 0; i < 3; i++) {
        q[i] = 0.0;
    }
    // and the real part to the scalar
    q[3] = scalar;
}

Quaternion::Quaternion(const Cartesian3& vector) {
    // copy vector part
    for (int i = 0; i < 3; i++) {
        q[i] = vector[i];
    }
    // set the real part to 0.0
    q[3] = 0.0;
}

Quaternion::Quaternion(const Homogeneous4& point) {
    // just copy the coordinates
    for (int i = 0; i < 4; i++) {
        q[i] = point[i];
    }
}

Quaternion::Quaternion(const Matrix4& matrix) {
    // copy rotation matrix
    // first, compute the trace of the matrix: the sum of the
    // diagonal elements (see convert() for coefficients)
    const float trace = matrix.coordinates[0][0] + matrix.coordinates[1][1]
                        + matrix.coordinates[2][2] + matrix.coordinates[3][3];
    // the trace should now contain 4 (1 - x^2 - y^2 - z^2)
    // and IF it is a pure rotation with no scaling, then
    // this is just 4 (w^2) since we will have a unit quaternion
    const float w = std::sqrt(trace * 0.25f);
    // now we can compute the vector component from symmetric
    // pairs of entries
    // (2yz + 2xw) - (2yz - 2xw) = 4 xw
    const float x = 0.25f * (matrix.coordinates[1][2] - matrix.coordinates[2][1]) / w;
    // (2xz + 2yw) - (2xz - 2yw) = 4 yw
    const float y = 0.25f * (matrix.coordinates[2][0] - matrix.coordinates[0][2]) / w;
    // (2xy + 2zw) - (2xy - 2zw) = 4 zw
    const float z = 0.25f * (matrix.coordinates[0][1] - matrix.coordinates[1][0]) / w;
    // now store them in the appropriate locations
    q[0] = x;
    q[1] = y;
    q[2] = z;
    q[3] = w;
}

Quaternion::Quaternion(const Cartesian3& axis, const float theta) {
    *this = Quaternion(axis.unit() * std::sin(theta)) + Quaternion(std::cos(theta));
}

float Quaternion::norm() const {
    return q[0] * q[0] + q[1] * q[1] +
           q[2] * q[2] + q[3] * q[3];
}

Quaternion Quaternion::unit() const {
    Quaternion result;
    // get the square root of the norm
    const float sqrtNorm = norm();
    // divide by it
    for (int i = 0; i < 4; i++) {
        result.q[i] = q[i] / sqrtNorm;
    }
    return result;
}

Quaternion Quaternion::conjugate() const {
    Quaternion result;
    for (int i = 0; i < 3; i++) {
        result.q[i] = q[i] * -1;
    }
    result.q[3] = q[3];
    return result;
}

Quaternion Quaternion::inverse() const {
    return conjugate() / norm();
}

Quaternion operator *(float scalar, const Quaternion& quat) {
    Quaternion result;
    for (int i = 0; i < 4; i++) {
        result.q[i] = scalar * quat.q[i];
    }
    return result;
}

Quaternion Quaternion::operator *(const float scalar) const {
    Quaternion result;
    for (int i = 0; i < 4; i++) {
        result.q[i] = q[i] * scalar;
    }
    return result;
}

Quaternion Quaternion::operator /(const float scalar) const {
    Quaternion result;
    for (int i = 0; i < 4; i++) {
        result.q[i] = q[i] / scalar;
    }
    return result;
}

Quaternion Quaternion::operator +(const Quaternion& other) const {
    Quaternion result;
    for (int i = 0; i < 4; i++) {
        result.q[i] = q[i] + other.q[i];
    }
    return result;
}

Quaternion Quaternion::operator -(const Quaternion& other) const {
    Quaternion result;
    for (int i = 0; i < 4; i++) {
        result.q[i] = q[i] - other.q[i];
    }
    return result;
}

Quaternion Quaternion::operator *(const Quaternion& other) const {
    Quaternion result;
    result.q[0] = +q[0] * other.q[3] // i 1
                  + q[1] * other.q[2] // j k
                  - q[2] * other.q[1] // k j
                  + q[3] * other.q[0]; // 1 i

    result.q[1] = -q[0] * other.q[2] // i k
                  + q[1] * other.q[3] // j 1
                  + q[2] * other.q[0] // k i
                  + q[3] * other.q[1]; // 1 j

    result.q[2] = +q[0] * other.q[1] // i j
                  - q[1] * other.q[0] // j i
                  + q[2] * other.q[3] // k 1
                  + q[3] * other.q[2]; // 1 k

    result.q[3] = -q[0] * other.q[0] // i i
                  - q[1] * other.q[1] // j j
                  - q[2] * other.q[2] // k k
                  + q[3] * other.q[3]; // 1 1
    return result;
}

Matrix4 Quaternion::asMatrix() const {
    Matrix4 result;

    // a quaternion (x y z w) is equivalent to the following matrix
    // | 1 - 2(y^2+z^2)          2(xy-wz)          2(xz+wy)    0 |
    // |       2(xy+wz)    1 - 2(x^2+z^2)          2(yz-wx)    0 |
    // |       2(xz-wy)          2(yz+wx)    1 - 2(x^2+y^2)    0 |
    // |              0                 0                 0    1 |
    const float xx = q[0] * q[0];
    const float xy = q[0] * q[1];
    const float xz = q[0] * q[2];
    const float xw = q[0] * q[3];

    const float yy = q[1] * q[1];
    const float yz = q[1] * q[2];
    const float yw = q[1] * q[3];

    const float zz = q[2] * q[2];
    const float zw = q[2] * q[3];

    result.coordinates[0][0] = 1 - 2 * (yy + zz);
    result.coordinates[0][1] = 2 * (xy - zw);
    result.coordinates[0][2] = 2 * (xz + yw);
    result.coordinates[0][3] = 0.0;

    result.coordinates[1][0] = 2 * (xy + zw);
    result.coordinates[1][1] = 1 - 2 * (xx + zz);
    result.coordinates[1][2] = 2 * (yz - xw);
    result.coordinates[1][3] = 0.0;

    result.coordinates[2][0] = 2 * (xz - yw);
    result.coordinates[2][1] = 2 * (yz + xw);
    result.coordinates[2][2] = 1 - 2 * (xx + yy);
    result.coordinates[2][3] = 0.0;

    result.coordinates[3][0] = 0.0;
    result.coordinates[3][1] = 0.0;
    result.coordinates[3][2] = 0.0;
    result.coordinates[3][3] = 1.0;

    return result;
}

std::ostream& operator <<(std::ostream& outStream, const Quaternion& quat) {
    return outStream << quat.q[0] << " " << quat.q[1] << " " << quat.q[2] << " " << quat.q[3] << std::endl;
}
