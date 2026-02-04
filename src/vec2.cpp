#include "vec2.h"


float math::DegToRad(float deg) {
    return deg / 180.0f * float(M_PI);
}

float math::RadToDeg(float rad) {
    return rad / float(M_PI) * 180.0f;
}

float math::Clamp(float value, float min, float max) {
    return std::fmin(std::fmax(value, min), max);
}
