#include <cmath>
#include "math.h"

#include <raymath.h>

float math::DegToRad(float deg) {
    return deg / 180.0f * float(M_PI);
}

float math::RadToDeg(float rad) {
    return rad / float(M_PI) * 180.0f;
}

float math::Clamp(float value, float min, float max) {
    return std::fmin(std::fmax(value, min), max);
}

float math::Vec2Angle(const math::Vec2& v) {
    return std::atan2(v.y, v.x);
}

math::Vec2 math::Vec2FromAngle(float radians) {
    return Vec2{std::cos(radians), std::sin(radians)};
}

math::Vec2 math::Vec2WithLength(const math::Vec2& v, float length) {
    return raylib::Vector2Normalize(v) * length;
}
