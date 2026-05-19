/*
** EPITECH PROJECT, 2026
** EngineSquared
** File description:
** RaylibPlugin
*/

#include "RaylibPlugin.hpp"

#include "Raylib.hpp"
#include "render/Renders.hpp"

namespace aot::plugin::raylib {
    void RaylibPlugin::Bind(void) {
        RegisterScheduler<render::PreRender>();
        RegisterScheduler<render::Render>();
        RegisterScheduler<render::PostRender>();

        this->GetCore()
            .SetSchedulerAfter<render::PreRender, Engine::Scheduler::Update>();
        this->GetCore().SetSchedulerAfter<render::Render, render::PreRender>();
        this->GetCore().SetSchedulerAfter<render::PostRender, render::Render>();
        this->GetCore()
            .SetSchedulerBefore<render::PostRender,
                                Engine::Scheduler::Shutdown>();
        // Startup -> (Update -> PreRender -> Render -> PostRender) -> Shutdown

        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &) {
            const int screenWidth = 800;
            const int screenHeight = 450;
            InitWindow(screenWidth, screenHeight, "Engine² x AOT");
            SetTargetFPS(60);
            DisableCursor();
        });

        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            if (WindowShouldClose()) {
                core.Stop();
            }
        });

        RegisterSystems<render::PreRender>(
            [](Engine::Core &) { BeginDrawing(); });

        RegisterSystems<render::PostRender>(
            [](Engine::Core &) { EndDrawing(); });

        RegisterSystems<Engine::Scheduler::Shutdown>(
            [](Engine::Core &) { CloseWindow(); });
    }
}  // namespace aot::plugin::raylib