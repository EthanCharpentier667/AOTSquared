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
#include "../component/equipement/Hook.hpp"
#include "../component/physics/ChildOffset.hpp"
#include "../component/physics/Collider.hpp"
#include "../component/physics/LineRenderer.hpp"
#include "../component/physics/Raycast.hpp"
#include "../plugin/render/Renders.hpp"
#include "../system/CameraSystem.hpp"
#include "../system/ControllerSystem.hpp"
#include "../system/LineRendererSystem.hpp"
#include "../system/RaycastSystem.hpp"
#include "../system/RigidbodySystem.hpp"
#include "Object.hpp"
#include "Raylib.hpp"
#include "Relationship.hpp"
#include "utils/AScene.hpp"

namespace aot::plugin::scene {
    class TestScene : public Scene::Utils::AScene {
      public:
        TestScene() = default;

      protected:
        void _onCreate(Engine::Core &core) final {
            Log::Info("TestScene created");

            auto player = core.CreateEntity();
            player.AddComponent<Object::Component::Transform>();
            auto &rigidBody = player.AddComponent<aot::character::Rigidbody>();
            auto &controller =
                player.AddComponent<aot::character::Controller>();
            auto &raycast = player.AddComponent<aot::physics::Raycast>();
            player.AddComponent<aot::physics::LineRenderer>();

            auto guntip = core.CreateEntity();
            Relationship::Utils::SetChildOf(guntip, player);
            guntip.AddComponent<aot::physics::ChildOffset>();
            player.AddComponent<aot::physics::ChildOffset>();

            auto &guntipTransform =
                guntip.AddComponent<Object::Component::Transform>();
            guntipTransform.SetPosition({rigidBody.position.x - 0.5f,
                                         rigidBody.position.y - 1.0f,
                                         rigidBody.position.z});
            player.AddComponent<aot::gear::Hook>(core, guntip);
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
            core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
                RaycastSystem);

            SetupGround(core);
            SetupTowers(core);

            raycast.layerMask =
                static_cast<uint32_t>(aot::physics::ColliderTag::Ground |
                                      aot::physics::ColliderTag::Tower |
                                      aot::physics::ColliderTag::Enemy);

            core.RegisterSystem<aot::plugin::render::Render>(
                [this](Engine::Core &core) {
                    auto view =
                        core.GetRegistry().view<aot::camera::RaylibCamera>();
                    for (auto entity : view) {
                        auto &camera =
                            view.get<aot::camera::RaylibCamera>(entity).camera;
                        BeginMode3D(camera);
                        DrawLevel(core);
                        EndMode3D();
                    }
                });
            core.RegisterSystem<aot::plugin::render::Render>(
                LineRendererSystem);
        }

        void _onDestroy(Engine::Core &core) final {
            (void)core;
        }

      private:
        void SetupGround(Engine::Core &core) {
            auto groundCollider = core.CreateEntity();
            auto &groundBox =
                groundCollider.AddComponent<aot::physics::BoxCollider>();
            groundBox.position = {0.0f, -0.5f, 0.0f};
            groundBox.size = {500.0f, 1.0f, 500.0f};
            groundBox.tag = aot::physics::ColliderTag::Ground;
        }

        void SetupTowers(Engine::Core &core) {
            Vector3 towerPositions[] = {
                {16.0f, 16.0f, 16.0f},
                {-16.0f, 16.0f, 16.0f},
                {-16.0f, 16.0f, -16.0f},
                {16.0f, 16.0f, -16.0f},
            };
            Vector3 towerSize = {16.0f, 32.0f, 16.0f};

            for (auto towerPos : towerPositions) {
                auto towerCollider = core.CreateEntity();
                auto &towerBox =
                    towerCollider.AddComponent<aot::physics::BoxCollider>();
                towerBox.position = towerPos;
                towerBox.size = towerSize;
                towerBox.tag = aot::physics::ColliderTag::Tower |
                               aot::physics::ColliderTag::Grappleable;
            }
        }

        void DrawLevel(Engine::Core &core) {
            const int floorExtent = 25;
            const float tileSize = 5.0f;
            const Color tileColor1 = (Color){150, 200, 200, 255};

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

            const Color towerColor = (Color){150, 200, 200, 255};

            auto &registry = core.GetRegistry();
            auto view = registry.view<aot::physics::BoxCollider>();

            for (auto entity : view) {
                auto &box = view.get<aot::physics::BoxCollider>(entity);

                if (!(static_cast<uint32_t>(box.tag) &
                      static_cast<uint32_t>(
                          aot::physics::ColliderTag::Tower))) {
                    continue;
                }

                DrawCubeV(box.position, box.size, towerColor);
                DrawCubeWiresV(box.position, box.size, DARKBLUE);
            }

            DrawSphere((Vector3){300.0f, 300.0f, 0.0f}, 100.0f,
                       (Color){255, 0, 0, 255});
        }
    };
}  // namespace aot::plugin::scene

#endif /* !TESTSCENE_HPP_ */
