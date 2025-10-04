#pragma once

#include <array>
#include <cmath>
#include <ostream>

namespace vision::math {

// 2D Vector
struct vector2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr vector2() = default;
    constexpr vector2(float x_val, float y_val) : x(x_val), y(y_val) {}

    // Arithmetic operators
    constexpr vector2 operator+(const vector2& other) const {
        return {x + other.x, y + other.y};
    }

    constexpr vector2 operator-(const vector2& other) const {
        return {x - other.x, y - other.y};
    }

    constexpr vector2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    constexpr vector2 operator/(float scalar) const {
        return {x / scalar, y / scalar};
    }

    // Assignment operators
    constexpr vector2& operator+=(const vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr vector2& operator-=(const vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr vector2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Utility functions
    [[nodiscard]] float length() const {
        return std::sqrt(x * x + y * y);
    }

    [[nodiscard]] float length_squared() const {
        return x * x + y * y;
    }

    [[nodiscard]] vector2 normalized() const {
        const float len = length();
        return len > 0.0f ? *this / len : vector2{};
    }

    [[nodiscard]] float dot(const vector2& other) const {
        return x * other.x + y * other.y;
    }

    [[nodiscard]] float distance_to(const vector2& other) const {
        return (*this - other).length();
    }
};

// 3D Vector
struct vector3 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr vector3() = default;
    constexpr vector3(float x_val, float y_val, float z_val) : x(x_val), y(y_val), z(z_val) {}

    // Arithmetic operators
    constexpr vector3 operator+(const vector3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    constexpr vector3 operator-(const vector3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    constexpr vector3 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar};
    }

    constexpr vector3 operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar};
    }

    // Assignment operators
    constexpr vector3& operator+=(const vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr vector3& operator-=(const vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr vector3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr vector3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Utility functions
    [[nodiscard]] float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    [[nodiscard]] float length_squared() const {
        return x * x + y * y + z * z;
    }

    [[nodiscard]] vector3 normalized() const {
        const float len = length();
        return len > 0.0f ? *this / len : vector3{};
    }

    [[nodiscard]] float dot(const vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    [[nodiscard]] vector3 cross(const vector3& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    [[nodiscard]] float distance_to(const vector3& other) const {
        return (*this - other).length();
    }
};

// 4D Vector
struct vector4 {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 0.0f;

    constexpr vector4() = default;
    constexpr vector4(float x_val, float y_val, float z_val, float w_val) 
        : x(x_val), y(y_val), z(z_val), w(w_val) {}

    // Arithmetic operators
    constexpr vector4 operator+(const vector4& other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    constexpr vector4 operator-(const vector4& other) const {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }

    constexpr vector4 operator*(float scalar) const {
        return {x * scalar, y * scalar, z * scalar, w * scalar};
    }

    constexpr vector4 operator/(float scalar) const {
        return {x / scalar, y / scalar, z / scalar, w / scalar};
    }

    // Utility functions
    [[nodiscard]] float length() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    [[nodiscard]] float dot(const vector4& other) const {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
};

// 3x3 Matrix
struct matrix3x3 {
    std::array<std::array<float, 3>, 3> m = {{{0}}};

    constexpr matrix3x3() = default;

    constexpr matrix3x3(const std::array<std::array<float, 3>, 3>& data) : m(data) {}

    // Identity matrix
    static constexpr matrix3x3 identity() {
        return {{
            {{1.0f, 0.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f}},
            {{0.0f, 0.0f, 1.0f}}
        }};
    }

    // Matrix multiplication
    matrix3x3 operator*(const matrix3x3& other) const {
        matrix3x3 result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                for (int k = 0; k < 3; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // Vector transformation
    vector3 operator*(const vector3& vec) const {
        return {
            m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z,
            m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z,
            m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z
        };
    }
};

// 3x4 Matrix (commonly used for transformations)
struct matrix3x4 {
    std::array<std::array<float, 4>, 3> m = {{{0}}};

    constexpr matrix3x4() = default;

    constexpr matrix3x4(const std::array<std::array<float, 4>, 3>& data) : m(data) {}

    // Vector transformation (homogeneous coordinates)
    vector3 operator*(const vector3& vec) const {
        return {
            m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3],
            m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3],
            m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3]
        };
    }
};

// 4x4 Matrix
struct matrix4x4 {
    std::array<std::array<float, 4>, 4> m = {{{0}}};

    constexpr matrix4x4() = default;

    constexpr matrix4x4(const std::array<std::array<float, 4>, 4>& data) : m(data) {}

    // Identity matrix
    static constexpr matrix4x4 identity() {
        return {{
            {{1.0f, 0.0f, 0.0f, 0.0f}},
            {{0.0f, 1.0f, 0.0f, 0.0f}},
            {{0.0f, 0.0f, 1.0f, 0.0f}},
            {{0.0f, 0.0f, 0.0f, 1.0f}}
        }};
    }

    // Matrix multiplication
    matrix4x4 operator*(const matrix4x4& other) const {
        matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // Vector transformation
    vector4 operator*(const vector4& vec) const {
        return {
            m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3] * vec.w,
            m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3] * vec.w,
            m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3] * vec.w,
            m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] * vec.w
        };
    }
};

// Stream operators for debugging
inline std::ostream& operator<<(std::ostream& os, const vector2& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

inline std::ostream& operator<<(std::ostream& os, const vector3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

inline std::ostream& operator<<(std::ostream& os, const vector4& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
}

}  // namespace vision::math
