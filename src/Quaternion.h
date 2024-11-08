#ifndef QUATERNION
#define QUATERNION

#include "Cartesian3.h"
#include "Homogeneous4.h"

#include "Matrix4.h"

class Quaternion {
public:
    // The values (x, y, z, w) for the quaternion w + x*i + y*j + z*k
    Homogeneous4 q;

    // Quaternion with (x, y, z, w) = (0, 0, 0, 1)
    Quaternion();

    Quaternion(float x, float y, float z, float w);

    // Set to a pure scalar value
    Quaternion(float scalar);

    // Set to a pure vector value
    Quaternion(const Cartesian3& vector);

    // Set to a homogeneous point
    Quaternion(const Homogeneous4& point);

    // Set to a rotation defined by a rotation matrix
    // matrix is assumed to be a rotation matrix
    Quaternion(const Matrix4& matrix);

    // Set to a rotation defined by an axis and angle
    Quaternion(const Cartesian3& axis, float theta);

    // Computes sum of squares
    float norm() const;

    Quaternion unit() const;

    Quaternion conjugate() const;

    Quaternion inverse() const;

    Quaternion operator *(float scalar) const;

    Quaternion operator /(float scalar) const;

    Quaternion operator +(const Quaternion& other) const;

    Quaternion operator -(const Quaternion& other) const;

    Quaternion operator *(const Quaternion& other) const;

    Matrix4 asMatrix() const;
};

Quaternion operator *(float scalar, const Quaternion& quat);

std::ostream& operator <<(std::ostream& outStream, const Quaternion& quat);

#endif
