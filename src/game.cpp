#include <vector>

#include "ray.h"
#include "game.h"
#include "vec2.h"
#include "missiles.h"
#include "random.h"


namespace {
    constexpr int kFontSize{10};

    raylib::RenderTexture2D screen;
    raylib::Sound explode_sound;

    bool debug{false};

    int mouse_x{0};
    int mouse_y{0};
    int mouse_buttons{0};

    int screen_x{0};
    int screen_y{0};

    float screen_shake_time{0.0f};
    float screen_shake_life{0.0f};

    std::vector<Missile> missiles;
    std::vector<MissileParticle> missile_particles;
    std::vector<ExplosionParticle> explosion_particles;
}


void FireMissile() {
    const float life = 5.0f;
    const float velocity = 150.0f;

    const int center_x = raylib::GetScreenWidth() / 2;
    const int center_y = raylib::GetScreenHeight() / 2;

    const float rand_x = (float)rnd::RandomInt(-center_x, center_x);
    const float rand_y = (float)rnd::RandomInt(-center_y, center_y);
    const float rand_v = (float)rnd::RandomInt(0, 20);
    const float rand_l = float((rnd::RandomInt(0, 100) / 100.0f) * 5.0f);

    Missile m;
    m.position = {float(center_x), float(center_y)};
    m.life = life + rand_l;

    m.velocity = {rand_x, rand_y};
    m.velocity.SetDistance(velocity + rand_v);

    missiles.push_back(m);
}

void Explode(const math::Vec2& pos, float dt) {
    const int n = 16;
    const float a = float(M_PI / n * 2);
    const float a_offset = math::DegToRad(float(rnd::RandomInt(0, 360.0f / n)));
    const float pow = 80.0f;

    float r = 0.0f;
    for (int i = 0; i < n; i += 1) {
        const float p_offset = float(rnd::RandomInt(0, 50));
        const float r_life = 0.9f + ((rnd::RandomInt(0, 100) / 100.0f) * 0.5f);

        ExplosionParticle p;
        p.position = pos;
        p.velocity.FromAngle(r + a_offset);
        p.velocity.SetDistance(pow + p_offset);
        p.life = r_life;

        explosion_particles.push_back(p);
        r += a;
    }
}

void ShakeScreen(float dt) {
    screen_shake_life = 0.5f;
    screen_shake_time = 0.0f;
}

bool UpdateMissile(Missile& m, float dt) {
    static const math::Vec2 drag {0.97f, 0.97f};
    static const math::Vec2 gravity {0.0f, -480.0f};
    static const float turn_radius = 200.0f;
    static const float dead_time = 2.0f;

    const int target_x = mouse_x;
    const int target_y = mouse_y;

    m.life -= dt;

    if (m.life < -dead_time) {
        Explode(m.position, dt);
        return false;
    }

    if (m.life <= 0.0f) {
        m.velocity.Multiply(drag);
        m.velocity.Subtract({gravity.x * dt, gravity.y * dt});
        m.position.Add({m.velocity.x * dt, m.velocity.y * dt});


        return true;
    }

    m.target = math::Vec2{float(target_x), float(target_y)};
    m.position.Add({m.velocity.x * dt, m.velocity.y * dt});

    math::Vec2 diff;
    diff = m.target;
    diff.Subtract(m.position);

    if (diff.DistanceSquared() <= 5.0f) {
        PlaySound(explode_sound);
        Explode(m.position, dt);
        ShakeScreen(dt);
        return false;
    }

    int r = rnd::RandomInt(0, 4);
    if (r == 0) {
        const float r_time = 0.4f + ((rnd::RandomInt(0, 100) / 100.0f) * 1.2f);

        MissileParticle p;
        p.life = r_time;
        p.position = m.position;

        float particle_angle = math::RadToDeg(m.velocity.Angle());
        particle_angle += float(rnd::RandomInt(-3, 3));

        p.velocity.FromAngle(math::DegToRad(-particle_angle));
        p.velocity.SetDistance(32.0f * dt);

        missile_particles.push_back(p);
    }

    const float target_angle = math::RadToDeg(diff.Angle());
    float current_angle = math::RadToDeg(m.velocity.Angle());
    float diff_angle = target_angle - current_angle;
    while (diff_angle < 0) {
        diff_angle += 360.0f;
    }

    if (diff_angle < 180.0f) {
        current_angle += std::min(turn_radius * dt, diff_angle);
    } else if (diff_angle > 180.0f) {
        current_angle -= std::min(turn_radius * dt, 360.0f - diff_angle);
    }

    math::Vec2 new_vel;
    new_vel.FromAngle(math::DegToRad(current_angle));
    new_vel.SetDistance(m.velocity.Distance());

    m.velocity = new_vel;

    return true;
}

void UpdateMissiles(float dt) {
    auto it = std::remove_if(begin(missiles), end(missiles), [dt] (Missile& m) {
        return !UpdateMissile(m, dt);
    });

    missiles.erase(it, end(missiles));

    std::sort(begin(missiles), end(missiles), [] (const Missile& m1, const Missile& m2) {
        return m1.position.x < m2.position.x;
    });
}

bool UpdateMissileParticle(MissileParticle& p, float dt) {
    static const math::Vec2 force {0.0f, -200.0f};
    static const math::Vec2 drag {0.98f, 0.98f};

    p.time += dt;

    if (p.time >= p.life) {
        return false;
    }

    p.velocity.Multiply(drag);
    p.velocity.Add({force.x * dt, force.y * dt});
    p.position.Add({p.velocity.x * dt, p.velocity.y * dt});

    return true;
}

void UpdateMissileParticles(float dt) {
    auto it = std::remove_if(begin(missile_particles), end(missile_particles),
        [dt] (MissileParticle& p) {
            return !UpdateMissileParticle(p, dt);
        });

    missile_particles.erase(it, end(missile_particles));
}

bool UpdateExplosionParticle(ExplosionParticle& p, float dt) {
    static const math::Vec2 force {0.0f, 100.0f};
    static const math::Vec2 drag {0.99f, 0.99f};

    p.time += dt;

    if (p.time >= p.life) {
        return false;
    }

    p.velocity.Multiply(drag);
    p.velocity.Add({force.x * dt, force.y * dt});
    p.position.Add({p.velocity.x * dt, p.velocity.y * dt});

    return true;
}

void UpdateExplosionParticles(float dt) {
    auto it = std::remove_if(begin(explosion_particles), end(explosion_particles),
        [dt] (ExplosionParticle& p) {
            return !UpdateExplosionParticle(p, dt);
        });

    explosion_particles.erase(it, end(explosion_particles));
}

void UpdateMouse(float dt) {
    // std::cout << "test" << dt << "\n";

    raylib::Vector2 mouse_pos = raylib::GetMousePosition();
    mouse_x = mouse_pos.x;
    mouse_y = mouse_pos.y;

    if (raylib::IsMouseButtonPressed(raylib::MOUSE_LEFT_BUTTON)) {
        FireMissile();
    }

    if (raylib::IsMouseButtonDown(raylib::MOUSE_RIGHT_BUTTON)) {
        const int count = 32;

        for (int i = 0; i < count; i += 1) {
            FireMissile();
        }
    }
}

void UpdateScreenShake(float dt) {
    static const float shake_amount = 5.0f;
    static const float shake_speed = 48.0f;

    if (screen_shake_time > screen_shake_life) {
        screen_x = 0;
        screen_y = 0;
        return;
    }

    screen_shake_time += dt;
    screen_y = int(std::sin(screen_shake_time * shake_speed) * shake_amount);
    screen_x = int(std::cos(screen_shake_life * shake_speed * 2) * shake_amount / 4.0f);
}

void DrawMissile(const Missile& m) {
    static const auto live_color = raylib::Color{ 255, 255, 0, 255 };
    static const auto dead_color = raylib::Color{ 37, 221, 245, 255 };
    static const auto line_color = raylib::Color{ 192, 192, 192, 255 };
    static const auto text_color = raylib::Color{ 255, 255, 255, 255 };

    static const int w = 4;
    static const int h = 4;
    static const int margin = 2;
    const int x{static_cast<int>(m.position.x)};
    const int y{static_cast<int>(m.position.y)};

    math::Vec2 v;
    v = m.velocity;
    v.Multiply({-1.0f, -1.0f});
    v.SetDistance(16.0f);

    const raylib::Color color = m.life < 0.0f ? dead_color : live_color;

    raylib::DrawRectangle(x - (w / 2), y - (h / 2), w, h, color);
    raylib::DrawLine(x, y, x + int(v.x), y + int(v.y), line_color);

    if (!debug) {
        return;
    }

    math::Vec2 t;
    t = m.target;
    t.Subtract(m.position);
    t.Subtract(m.velocity);

    char text[256];
    snprintf(text, sizeof(text), "p:(% 3d, % 3d)\nv:(% 3d, %3d)\na:% 4.2f\nta:% 4.2f",
             x, y,
             int(m.velocity.x), int(m.velocity.y),
             math::RadToDeg(m.velocity.Angle()),
             math::RadToDeg(t.Angle()));

    DrawText(text, x + margin, y + margin, kFontSize, text_color);
}

void DrawMissiles() {
    for (auto& m : missiles) {
        DrawMissile(m);
    }
}

void DrawMissileParticle(const MissileParticle& p) {
    const auto color = raylib::Color{ 178, 178, 178, 255 };

    const int min = 2;
    const int max = 8;
    const int w = min + int((p.time / p.life) * (max - min));
    const int h = w;
    const int x = int(p.position.x);
    const int y = int(p.position.y);

    raylib::DrawRectangle(x - (w / 2), y - (h / 2), w, h, color);
}

void DrawMissleParticles() {
    for (auto& p : missile_particles) {
        DrawMissileParticle(p);
    }
}

void DrawExplosionParticle(const ExplosionParticle& p) {
    const auto color = raylib::Color{ 255, 255, 255, 255 };
    const float length = math::Clamp(p.velocity.Distance() / 200.0f, 0.0f, 1.0f);

    math::Vec2 d;
    d = p.velocity;
    d.Multiply({-1.0f, -1.0f});
    d.SetDistance(length * 12.0f);

    math::Vec2 v;
    v = p.position;
    v.Add(d);

    const int x1 = int(p.position.x);
    const int y1 = int(p.position.y);
    const int x2 = int(v.x);
    const int y2 = int(v.y);

    DrawLine(x1, y1, x2, y2, color);

    const int w = 2;
    const int h = w;
    const int x = x1 - (w / 2);
    const int y = y1 - (h / 2);

    DrawRectangle(x + raylib::GetScreenWidth(), y + raylib::GetScreenHeight(), w, h, raylib::Color{ 190, 120, 0, 255 });

}

void DrawExplosionParticles() {
    for (auto& p : explosion_particles) {
        DrawExplosionParticle(p);
    }
}

void DrawCrosshair() {
    static const auto color = raylib::Color{ 123, 175, 201, 255 };

    const int x = mouse_x;
    const int y = mouse_y;

    DrawLine(x, 0, x, raylib::GetScreenHeight(), color);
    DrawLine(0, y, raylib::GetScreenWidth(), y, color);
}

void DrawArrow() {
    const auto color = raylib::Color{ 213, 246, 221, 255 };

    const int center_x = raylib::GetScreenWidth() / 2;
    const int center_y = raylib::GetScreenHeight() / 2;

    const int target_x = mouse_x;
    const int target_y = mouse_y;

    math::Vec2 v {float(target_x - center_x), float(target_y - center_y)};
    v.SetDistance(24.0f);

    DrawLine(center_x, center_y, center_x + static_cast<int>(v.x), center_y + static_cast<int>(v.y), color);
}

void DrawFPS(int frame_time, int fps) {
    static const auto color = raylib::Color{ 255, 255, 255, 255 };
    static const int margin = 8;

    char text[128];
    snprintf(text, sizeof(text), "% 4d ms/frame\n% 4d frames/sec", frame_time, fps);

    int width = 100;
    int height = 24;

    DrawText(text, raylib::GetScreenWidth() - width - margin, raylib::GetScreenHeight() - height - margin, kFontSize, color);
}

void DrawParticleInfo() {
    static const auto color = raylib::Color{ 255, 255, 255, 255 };
    static const int margin = 8;
    const int m_count = (int)missiles.size();
    const int s_count = (int)missile_particles.size();
    const int p_count = (int)explosion_particles.size();

    char text[128];
    snprintf(text, sizeof(text),
             "% 4d missiles\n% 4d smoke\n% 4d sparks",
             m_count, s_count, p_count);

    DrawText(text, margin, raylib::GetScreenHeight() - 40 - margin, kFontSize, color);
}

void DrawMouseInfo() {
    const auto color = raylib::Color{ 255, 255, 255, 255 };
    const int margin = 8;

    const int mouse_window_x = mouse_x;
    const int mouse_window_y = mouse_y;

    const int center_x = raylib::GetScreenWidth() / 2;
    const int center_y = raylib::GetScreenHeight() / 2;


    math::Vec2 v {float(mouse_window_x - center_x), float(mouse_window_y - center_y)};
    const int angle = (int)math::RadToDeg(v.Angle());

    char text[128];
    snprintf(text, sizeof(text),
             "mouse position: (% 3d, %3d)\nmouse angle: % 3d deg\nbutton: % 3d",
             mouse_x, mouse_y, angle, mouse_buttons);

    int width = raylib::MeasureText(text, kFontSize);

    DrawText(text, raylib::GetScreenWidth() - width - margin, margin, kFontSize, color);
}

void DrawGrid() {
    static const auto color = raylib::Color{ 148, 148, 148, 255 };
    static const int grid_size = 60;

    const int half_width = raylib::GetScreenWidth() / 2;
    const int half_height = raylib::GetScreenHeight() / 2;

    const int grid_x_count = 2 * ((half_width / grid_size) + 1);
    const int grid_y_count = 2 * ((half_height / grid_size) + 1);

    const int start_x = half_width - ((grid_x_count / 2) * grid_size);
    const int start_y = half_height - ((grid_y_count / 2) * grid_size);

    for (int j = 0; j < grid_y_count; j += 1) {
        const int y = start_y + (j * grid_size);

        for (int i = 0; i < grid_x_count; i += 1) {
            const int x = start_x + (i * grid_size);
            const int c = (i + j) % 2;

            if (c == 0) {
                DrawRectangle(x, y, grid_size, grid_size, color);
            }
        }
    }
}


bool Game::OnInit() {
    screen = raylib::LoadRenderTexture(raylib::GetScreenWidth(), raylib::GetScreenHeight());
    explode_sound = raylib::LoadSound("resources/explode.mp3");
    return true;
}

void Game::OnCleanup() {
    raylib::UnloadSound(explode_sound);
    raylib::UnloadRenderTexture(screen);
}

void Game::OnUpdate(float dt) {
    UpdateMouse(dt);
    UpdateScreenShake(dt);

    UpdateMissiles(dt);
    UpdateMissileParticles(dt);
    UpdateExplosionParticles(dt);
}

void Game::OnRender() {
    const float screen_width = raylib::GetScreenWidth();
    const float screen_height = raylib::GetScreenHeight();

    raylib::BeginTextureMode(screen);
    raylib::ClearBackground({ 127, 127, 127, 255 });

    DrawGrid();

    DrawMissleParticles();
    DrawExplosionParticles();
    DrawMissiles();

    raylib::EndTextureMode();
    raylib::BeginDrawing();
    raylib::ClearBackground({ 64, 64, 64, 255 });

    raylib::DrawTexturePro(
        screen.texture,
        raylib::Rectangle{ 0, 0, screen_width, -screen_height },
        raylib::Rectangle{ static_cast<float>(screen_x), static_cast<float>(screen_y), screen_width, screen_height },
        raylib::Vector2{ 0, 0 },
        0.0f,
        raylib::WHITE
    );

    DrawCrosshair();
    DrawArrow();

    DrawFPS(GetFrameTime(), GetFPS());
    DrawParticleInfo();
    DrawMouseInfo();

    raylib::EndDrawing();
}

bool Game::OnProcessEvents() {
    if (raylib::IsKeyDown(raylib::KEY_D)) {
        debug = !debug;
    }
    return true;
}
