#include "Cartesian3.h"

#include <cmath>
#include <iomanip>

Cartesian3::Cartesian3()
    : x(0.0f), y(0.0f), z(0.0f) {
}

Cartesian3::Cartesian3(const float x, const float y, const float z)
    : x(x), y(y), z(z) {
}

Cartesian3::Cartesian3(const Cartesian3& other)
    : x(other.x), y(other.y), z(other.z) {
}

bool Cartesian3::operator ==(const Cartesian3& other) const {
    return std::abs(x - other.x) < std::numeric_limits<float>::epsilon() &&
           std::abs(y - other.y) < std::numeric_limits<float>::epsilon() &&
           std::abs(z - other.z) < std::numeric_limits<float>::epsilon();
}

Cartesian3 Cartesian3::operator-() const {
    return Cartesian3(-x, -y, -z);
}

Cartesian3 Cartesian3::operator +(const Cartesian3& other) const {
    return Cartesian3(x + other.x, y + other.y, z + other.z);
}

Cartesian3 Cartesian3::operator -(const Cartesian3& other) const {
    return Cartesian3(x - other.x, y - other.y, z - other.z);
}

Cartesian3 Cartesian3::operator *(const float factor) const {
    return Cartesian3(x * factor, y * factor, z * factor);
}

Cartesian3 Cartesian3::operator /(const float factor) const {
    return Cartesian3(x / factor, y / factor, z / factor);
}

Cartesian3& Cartesian3::operator+=(const Cartesian3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

float Cartesian3::dot(const Cartesian3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Cartesian3 Cartesian3::cross(const Cartesian3& other) const {
    return Cartesian3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

float Cartesian3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Cartesian3 Cartesian3::unit() const {
    const float length = std::sqrt(x * x + y * y + z * z);
    return Cartesian3(x / length, y / length, z / length);
}

float& Cartesian3::operator [](const int index) {
    switch (index) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        // error case, choose to default to x
        default:
            return x;
    }
}

const float& Cartesian3::operator [](const int index) const {
    switch (index) {
        // switch on index
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        // error case, choose to default to x
        default:
            return x;
    }
}

Cartesian3 operator *(const float factor, const Cartesian3& right) {
    return right * factor;
}

std::istream& operator >>(std::istream& inStream, Cartesian3& value) {
    return inStream >> value.x >> value.y >> value.z;
}

std::ostream& operator <<(std::ostream& outStream, const Cartesian3& value) {
    return outStream << std::setprecision(4)
           << value.x << " " << value.y << " " << value.z;
}

std::string Cartesian3::toString() const {
    std::stringstream stream;
    // Dereference to re-use operator<<
    stream << *this;
    return "[" + stream.str() + "]";
}
