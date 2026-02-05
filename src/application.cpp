#include <spdlog/spdlog.h>
#include "application.h"
#include "ray.h"


namespace {
    constexpr int kFontSize{10};
    constexpr int kScreenWidth{800};
    constexpr int kScreenHeight{600};
    const char* kWindowTitle = "Homing missiles!!11";

    int frame_time{0};
    int fps{0};
}

static void RaylibSpdlogCallback(int log_level, const char *text, va_list args) {
    static char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), text, args);

    switch (log_level) {
        case raylib::LOG_TRACE: spdlog::trace(buffer); break;
        case raylib::LOG_DEBUG: spdlog::debug(buffer); break;
        case raylib::LOG_INFO: spdlog::info(buffer); break;
        case raylib::LOG_WARNING: spdlog::warn(buffer); break;
        case raylib::LOG_ERROR: spdlog::error(buffer); break;
        case raylib::LOG_FATAL: spdlog::critical(buffer); break;
        default: break;
    }
}

int Application::Run() {
    if (!Init()) {
        Shutdown();
        return 1;
    }

    MainLoop();
    Shutdown();
    return 0;
}

bool Application::Init() {
    spdlog::info("Initializing application");

    raylib::SetTraceLogCallback(RaylibSpdlogCallback);

    raylib::InitWindow(kScreenWidth, kScreenHeight, kWindowTitle);
    raylib::InitAudioDevice();
    raylib::SetExitKey(raylib::KEY_ESCAPE);
    raylib::SetTargetFPS(60);
    return OnInit();
}

void Application::Shutdown() {
    OnCleanup();
    raylib::CloseAudioDevice();
    raylib::CloseWindow();
}

void Application::MainLoop() {
    const double dt {0.01f};
    double accumulator {0.0f};
    while (!raylib::WindowShouldClose()) {
        if (!OnProcessEvents()) {
            break;
        }

        float time = raylib::GetFrameTime();
        accumulator += time;

        UpdateFPS(time);

        while (accumulator >= dt) {
            OnUpdate(dt);
            accumulator -= dt;
        }

        OnRender();
    }
}

void Application::UpdateFPS(float dt) {
    frame_time = int(dt * 1000);
    fps = int(1.0f / dt);
}

int Application::GetFrameTime() const {
    return frame_time;
}

int Application::GetFPS() const {
    return fps;
}

int Application::GetScreenWidth() const {
    return raylib::GetScreenWidth();
}

int Application::GetScreenHeight() const {
    return raylib::GetScreenHeight();
}
