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
#include "../component/render/CubeMesh.hpp"
#include "../component/render/PlaneMesh.hpp"
#include "../component/render/SphereMesh.hpp"
#include "../plugin/render/Renders.hpp"
#include "../system/LineRendererSystem.hpp"
#include "../system/RaycastSystem.hpp"
#include "../system/camera/CameraSystem.hpp"
#include "../system/physics/ControllerSystem.hpp"
#include "../system/physics/HookSystem.hpp"
#include "../system/physics/RigidbodySystem.hpp"
#include "../system/render/ColliderGizmoSystem.hpp"
#include "../system/render/CubeMeshSystem.hpp"
#include "../system/render/PlaneMeshSystem.hpp"
#include "../system/render/SphereMeshSystem.hpp"
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
            auto &playerTransform =
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
            auto &hook = player.AddComponent<aot::gear::Hook>();
            hook.anchor = guntip;
            auto cameraEntity = core.CreateEntity();
            Relationship::Utils::SetChildOf(cameraEntity, player);
            auto &cameraTransform =
                cameraEntity.AddComponent<Object::Component::Transform>();
            auto &cameraOffset =
                cameraEntity.AddComponent<aot::physics::ChildOffset>();
            cameraOffset.positionOffset = {0.0f, BOTTOM_HEIGHT + STAND_HEIGHT,
                                           0.0f};
            cameraTransform.SetPosition(
                {playerTransform.GetPosition().x,
                 playerTransform.GetPosition().y + BOTTOM_HEIGHT + STAND_HEIGHT,
                 playerTransform.GetPosition().z});
            auto sphereEntity = core.CreateEntity();

            Camera cam = {
                .position = {playerTransform.GetPosition().x,
                             playerTransform.GetPosition().y + BOTTOM_HEIGHT +
                                 STAND_HEIGHT,
                             playerTransform.GetPosition().z},
                .target = {playerTransform.GetPosition().x,
                           playerTransform.GetPosition().y + BOTTOM_HEIGHT +
                               STAND_HEIGHT,
                           playerTransform.GetPosition().z - 1.0f},
                .up = {0.0f, 1.0f, 0.0f},
                .fovy = 60.0f,
                .projection = CAMERA_PERSPECTIVE,
            };
            auto &raylibCamera =
                cameraEntity.AddComponent<aot::camera::RaylibCamera>(cam);
            raylibCamera.headLerp = STAND_HEIGHT;
            auto &sphereTransform =
                sphereEntity.AddComponent<Object::Component::Transform>();
            sphereTransform.SetPosition({300.0f, 300.0f, 0.0f});
            sphereEntity.AddComponent<aot::geometry::SphereMesh>(100.0f, 16u,
                                                                 24u, RED);

            auto &fixedTimeScheduler =
                core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>();
            fixedTimeScheduler.SetTickRate(1.0f / 120.0f);
            core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
                CameraSystem);
            core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
                RaycastSystem);

            aot::physics::RegisterControllerSystems(core);
            aot::physics::RegisterRigidbodySystems(core);
            aot::physics::RegisterHookSystems(core);

            SetupGround(core);
            SetupTowers(core);

            raycast.layerMask =
                static_cast<uint32_t>(aot::physics::ColliderTag::Ground |
                                      aot::physics::ColliderTag::Tower |
                                      aot::physics::ColliderTag::Enemy);

            core.RegisterSystem<aot::plugin::render::Render>(PlaneMeshSystem);
            core.RegisterSystem<aot::plugin::render::Render>(CubeMeshSystem);
            core.RegisterSystem<aot::plugin::render::Render>(SphereMeshSystem);
            core.RegisterSystem<aot::plugin::render::Render>(
                LineRendererSystem);
            core.RegisterSystem<aot::plugin::render::Render>(
                physics::ColliderGizmoSystem);
        }

        void _onDestroy(Engine::Core &core) final {
            (void)core;
        }

      private:
        void SetupGround(Engine::Core &core) {
            auto groundCollider = core.CreateEntity();
            auto &groundBox =
                groundCollider.AddComponent<aot::physics::BoxCollider>(true);
            auto &groundTransform =
                groundCollider.AddComponent<Object::Component::Transform>();
            groundCollider.AddComponent<aot::geometry::PlaneMesh>(
                500.0f, 500.0f, 1u, 1u, (Color){150, 200, 200, 255});
            groundBox.position = {0.0f, -0.5f, 0.0f};
            groundBox.size = {500.0f, 1.0f, 500.0f};
            groundBox.tag = aot::physics::ColliderTag::Ground;
            groundTransform.SetPosition({0.0f, -0.5f, 0.0f});
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
                    towerCollider.AddComponent<aot::physics::BoxCollider>(true);
                auto &towerTransform =
                    towerCollider.AddComponent<Object::Component::Transform>();
                towerCollider.AddComponent<aot::geometry::CubeMesh>(towerSize,
                                                                    DARKBLUE);
                towerBox.position = towerPos;
                towerBox.size = towerSize;
                towerBox.tag = aot::physics::ColliderTag::Tower |
                               aot::physics::ColliderTag::Grappleable;
                towerTransform.SetPosition(towerPos.x, towerPos.y, towerPos.z);
            }
        }
    };
}  // namespace aot::plugin::scene

#endif /* !TESTSCENE_HPP_ */
