/*
** EPITECH PROJECT, 2026
** EngineSquared
** File description:
** RaylibPlugin
*/

#include "RaylibPlugin.hpp"

#include "Raylib.hpp"
#include "component/camera/RaylibCamera.hpp"
#include "render/Renders.hpp"

namespace aot::plugin::raylib {
    static bool render3DActive = false;

    void RaylibPlugin::Bind(void) {
        RegisterScheduler<render::PreRender>();
        RegisterScheduler<render::PreRender3D>();
        RegisterScheduler<render::Render>();
        RegisterScheduler<render::PostRender3D>();
        RegisterScheduler<render::PostRender>();

        this->GetCore()
            .SetSchedulerAfter<render::PreRender, Engine::Scheduler::Update>();
        this->GetCore()
            .SetSchedulerAfter<render::PreRender3D, render::PreRender>();
        this->GetCore()
            .SetSchedulerAfter<render::Render, render::PreRender3D>();
        this->GetCore()
            .SetSchedulerAfter<render::PostRender3D, render::Render>();
        this->GetCore()
            .SetSchedulerAfter<render::PostRender, render::PostRender3D>();
        this->GetCore()
            .SetSchedulerBefore<render::PostRender,
                                Engine::Scheduler::Shutdown>();
        // Startup -> (Update -> PreRender -> PreRender3D -> Render ->
        // PostRender3D -> PostRender) -> Shutdown

        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &) {
            const int screenWidth = 800;
            const int screenHeight = 450;
            InitWindow(screenWidth, screenHeight, "Engine² x AOT");
            SetTargetFPS(60);
        });

        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            if (WindowShouldClose()) {
                core.Stop();
            }
        });

        RegisterSystems<render::PreRender>(
            [](Engine::Core &) { BeginDrawing(); });

        RegisterSystems<render::PreRender3D>([](Engine::Core &core) {
            auto &registry = core.GetRegistry();
            auto cameraView = registry.view<aot::camera::RaylibCamera>();
            cameraView.each([&](auto &raylibCamera) {
                BeginMode3D(raylibCamera.camera);
                render3DActive = true;
            });
        });

        RegisterSystems<render::PostRender3D>([](Engine::Core &) {
            if (!render3DActive)
                return;

            EndMode3D();
            render3DActive = false;
        });

        RegisterSystems<render::PostRender>(
            [](Engine::Core &) { EndDrawing(); });

        RegisterSystems<Engine::Scheduler::Shutdown>(
            [](Engine::Core &) { CloseWindow(); });
    }
}  // namespace aot::plugin::raylib