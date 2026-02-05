#ifndef __GAME_MATH_H__
#define __GAME_MATH_H__

#include <cmath>
#include "ray.h"


namespace math {
    using Vec2 = raylib::Vector2;

    float DegToRad(float deg);
    float RadToDeg(float rad);
    float Clamp(float value, float min = 0.0f, float max = 1.0f);
    float Vec2Angle(const Vec2& v);

    Vec2 Vec2FromAngle(float radians);
    Vec2 Vec2WithLength(const Vec2& v, float length);
}


#endif//__GAME_MATH_H__