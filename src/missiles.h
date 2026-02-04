#ifndef __MISSLES_H__
#define __MISSLES_H__

#include "vec2.h"


struct Missile {
    Vec2 position {0.0f, 0.0f};
    Vec2 velocity {0.0f, 0.0f};
    Vec2 target {0.0f, 0.0f};
    float life {0.0f};
    float stage {0.0f};
    float stage_dir {-1.0f};
    float stage_max {0.5f};
    float stage_min {-0.02f};
};

struct MissileParticle {
    Vec2 position {0.0f, 0.0f};
    Vec2 velocity {0.0f, 0.0f};
    float life {0.7f};
    float time {0.0f};
};

struct ExplosionParticle {
    Vec2 position {0.0f, 0.0f};
    Vec2 velocity {0.0f, 0.0f};
    float life {1.2f};
    float time {0.0f};
};


#endif//__MISSLES_H__
