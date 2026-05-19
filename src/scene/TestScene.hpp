/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** TestScene
*/

#ifndef TESTSCENE_HPP_
#define TESTSCENE_HPP_

#include "../component/camera/RaylibCamera.hpp"
#include "../component/character/Controller.hpp"
#include "../component/character/Rigidbody.hpp"
#include "../plugin/render/Renders.hpp"
#include "../system/CameraSystem.hpp"
#include "../system/ControllerSystem.hpp"
#include "../system/RigidbodySystem.hpp"
#include "Raylib.hpp"
#include "utils/AScene.hpp"

namespace aot::plugin::scene {
    class TestScene : public Scene::Utils::AScene {
      public:
        TestScene() = default;

      protected:
        void _onCreate(Engine::Core &core) final {
            Log::Info("TestScene created");

            auto player = core.CreateEntity();
            auto &rigidBody = player.AddComponent<aot::character::Rigidbody>();
            auto &controller =
                player.AddComponent<aot::character::Controller>();

            auto cameraEntity = core.CreateEntity();

            Camera cam = {
                .position = {rigidBody.position.x,
                             (BOTTOM_HEIGHT + controller.headLerp),
                             rigidBody.position.z},
                .target = {rigidBody.position.x,
                           (BOTTOM_HEIGHT + controller.headLerp),
                           rigidBody.position.z - 1.0f},
                .up = {0.0f, 1.0f, 0.0f},
                .fovy = 60.0f,
                .projection = CAMERA_PERSPECTIVE,
            };
            cameraEntity.AddComponent<aot::camera::RaylibCamera>(cam);

            auto &fixedTimeScheduler =
                core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>();
            fixedTimeScheduler.SetTickRate(1.0f / 120.0f);

            core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
                ControllerSystem);
            core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
                RigidbodySystem);
            core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
                CameraSystem);

            core.RegisterSystem<aot::plugin::render::Render>(
                [this](Engine::Core &core) {
                    auto view =
                        core.GetRegistry().view<aot::camera::RaylibCamera>();
                    for (auto entity : view) {
                        auto &camera =
                            view.get<aot::camera::RaylibCamera>(entity).camera;
                        BeginMode3D(camera);
                        DrawLevel();
                        EndMode3D();
                    }
                });
        }
        void _onDestroy(Engine::Core &core) final {
        }

      private:
        void DrawLevel(void) {
            const int floorExtent = 25;
            const float tileSize = 5.0f;
            const Color tileColor1 = (Color){150, 200, 200, 255};

            // Floor tiles
            for (int y = -floorExtent; y < floorExtent; y++) {
                for (int x = -floorExtent; x < floorExtent; x++) {
                    if ((y & 1) && (x & 1)) {
                        DrawPlane((Vector3){x * tileSize, 0.0f, y * tileSize},
                                  (Vector2){tileSize, tileSize}, tileColor1);
                    } else if (!(y & 1) && !(x & 1)) {
                        DrawPlane((Vector3){x * tileSize, 0.0f, y * tileSize},
                                  (Vector2){tileSize, tileSize}, LIGHTGRAY);
                    }
                }
            }

            const Vector3 towerSize = (Vector3){16.0f, 32.0f, 16.0f};
            const Color towerColor = (Color){150, 200, 200, 255};

            Vector3 towerPos = (Vector3){16.0f, 16.0f, 16.0f};
            DrawCubeV(towerPos, towerSize, towerColor);
            DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

            towerPos.x *= -1;
            DrawCubeV(towerPos, towerSize, towerColor);
            DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

            towerPos.z *= -1;
            DrawCubeV(towerPos, towerSize, towerColor);
            DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

            towerPos.x *= -1;
            DrawCubeV(towerPos, towerSize, towerColor);
            DrawCubeWiresV(towerPos, towerSize, DARKBLUE);

            // Red sun
            DrawSphere((Vector3){300.0f, 300.0f, 0.0f}, 100.0f,
                       (Color){255, 0, 0, 255});
        }
    };
}  // namespace aot::plugin::scene

#endif /* !TESTSCENE_HPP_ */
