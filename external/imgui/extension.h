#pragma once
#include <imgui.h>

inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

inline ImVec2 operator*(const ImVec2& vec, float scalar)
{
    return {vec.x * scalar, vec.y * scalar};
}
