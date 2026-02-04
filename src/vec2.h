#ifndef __VEC2_H__
#define __VEC2_H__

#include <cmath>


namespace math {

    float DegToRad(float deg);
    float RadToDeg(float rad);
    float Clamp(float value, float min, float max);

    struct Vec2 {
        float x{0.0f};
        float y{0.0f};

        inline void FromAngle(float radians) {
            x = std::cos(radians);
            y = std::sin(radians);
        }

        inline void Add(const Vec2 &v) {
            x += v.x;
            y += v.y;
        }

        inline void Subtract(const Vec2 &v) {
            x -= v.x;
            y -= v.y;
        }

        inline void Multiply(const Vec2 &v) {
            x *= v.x;
            y *= v.y;
        }

        inline void Divide(const Vec2 &v) {
            x /= v.x;
            y /= v.y;
        }

        inline float DistanceSquared() const {
            return (x * x) + (y * y);
        }

        inline float Distance() const {
            return std::sqrt(DistanceSquared());
        }

        inline void Normalize() {
            float dist = Distance();
            Divide({dist, dist});
        }

        inline void SetDistance(float distance) {
            Normalize();
            Multiply({distance, distance});
        }

        inline float Angle() const {
            return std::atan2(y, x);
        }
    };
}

#endif//__VEC2_H__
