/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** TestScene
*/

#include "TestScene.hpp"

#include "../component/physics/ChildOffsetUils.hpp"

namespace aot::plugin::scene {

    void TestScene::_onCreate(Engine::Core &core) {
        Log::Info("TestScene created");

        auto player = SetupPlayer(core);
        auto camera = SetupCamera(core, player);

        auto &fixedTimeScheduler =
            core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>();
        fixedTimeScheduler.SetTickRate(1.0f / 120.0f);

        aot::physics::RegisterControllerSystems(core);
        aot::physics::RegisterRigidbodySystems(core);
        aot::physics::RegisterHookSystems(core);
        core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
            ChildFollowParentSystem);
        core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
            CollisionSystem);
        core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(CameraSystem);

        SetupGround(core);
        SetupTowers(core);
        SetupSun(core);

        core.RegisterSystem<aot::plugin::render::Render>(PlaneMeshSystem);
        core.RegisterSystem<aot::plugin::render::Render>(CubeMeshSystem);
        core.RegisterSystem<aot::plugin::render::Render>(SphereMeshSystem);
        core.RegisterSystem<aot::plugin::render::Render>(LineRendererSystem);
        core.RegisterSystem<aot::plugin::render::Render>(
            physics::ColliderGizmoSystem);
    }

    void TestScene::_onDestroy(Engine::Core &core) {
        (void)core;
    }

    Engine::Entity TestScene::SetupCamera(Engine::Core &core,
                                          Engine::Entity player) {
        auto cameraEntity = core.CreateEntity();
        auto &playerTransform =
            player.GetComponents<Object::Component::Transform>();
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
        return cameraEntity;
    }

    Engine::Entity TestScene::SetupPlayer(Engine::Core &core) {
        auto player = core.CreateEntity();
        auto &playerTransform =
            player.AddComponent<Object::Component::Transform>();
        auto &rigidBody = player.AddComponent<aot::character::Rigidbody>();
        auto &controller = player.AddComponent<aot::character::Controller>();

        auto &playerCollider =
            player.AddComponent<aot::physics::CapsuleCollider>(false);
        playerCollider.radius = 0.5f;
        playerCollider.height = STAND_HEIGHT + (BOTTOM_HEIGHT * 2.5f);
        playerCollider.tag = aot::physics::ColliderTag::Player;
        playerCollider.offset = {0.0f, playerCollider.height / 2.0f, 0.0f};

        // Setup right hook and guntip
        auto rightGuntip = core.CreateEntity();
        rightGuntip.AddComponent<aot::physics::LineRenderer>();

        auto &rightGuntipTransform =
            rightGuntip.AddComponent<Object::Component::Transform>();
        rightGuntipTransform.SetPosition({rigidBody.position.x + 1.0f,
                                          rigidBody.position.y - 0.5f,
                                          rigidBody.position.z});
        aot::physics::ChildOffsetUils::makeChildFollowParent(rightGuntip,
                                                             player);

        auto &rightHook = rightGuntip.AddComponent<aot::gear::Hook>();
        rightHook.key = KEY_E;

        // Setup left hook and guntip
        auto leftGuntip = core.CreateEntity();
        leftGuntip.AddComponent<aot::physics::LineRenderer>();

        auto &leftGuntipTransform =
            leftGuntip.AddComponent<Object::Component::Transform>();
        leftGuntipTransform.SetPosition({rigidBody.position.x - 1.0f,
                                         rigidBody.position.y - 0.5f,
                                         rigidBody.position.z});
        aot::physics::ChildOffsetUils::makeChildFollowParent(leftGuntip,
                                                             player);

        auto &leftHook = leftGuntip.AddComponent<aot::gear::Hook>();
        leftHook.key = KEY_Q;

        return player;
    }

    void TestScene::SetupSun(Engine::Core &core) {
        auto sphereEntity = core.CreateEntity();
        auto &sphereTransform =
            sphereEntity.AddComponent<Object::Component::Transform>();
        sphereTransform.SetPosition({300.0f, 300.0f, 0.0f});
        sphereEntity.AddComponent<aot::geometry::SphereMesh>(100.0f, 16u, 24u,
                                                             RED);
    }

    void TestScene::SetupGround(Engine::Core &core) {
        auto ground = core.CreateEntity();
        auto &groundTransform =
            ground.AddComponent<Object::Component::Transform>();
        groundTransform.SetPosition({0.0f, -0.5f, 0.0f});
        ground.AddComponent<aot::geometry::PlaneMesh>(
            500.0f, 500.0f, 1u, 1u, (Color){150, 200, 200, 255});
        auto &groundCollider =
            ground.AddComponent<aot::physics::BoxCollider>(true);
        groundCollider.position = {0.0f, -0.5f, 0.0f};
        groundCollider.size = {500.0f, 1.0f, 500.0f};
        groundCollider.tag = aot::physics::ColliderTag::Ground;
    }

    void TestScene::SetupTowers(Engine::Core &core) {
        Vector3 towerPositions[] = {
            {16.0f, 16.0f, 16.0f},
            {-16.0f, 16.0f, 16.0f},
            {-16.0f, 16.0f, -16.0f},
            {16.0f, 16.0f, -16.0f},
        };
        Vector3 towerSize = {16.0f, 32.0f, 16.0f};

        for (auto towerPos : towerPositions) {
            auto tower = core.CreateEntity();
            auto &towerCollider =
                tower.AddComponent<aot::physics::BoxCollider>(true);
            auto &towerTransform =
                tower.AddComponent<Object::Component::Transform>();
            tower.AddComponent<aot::geometry::CubeMesh>(towerSize, DARKBLUE);
            towerCollider.position = towerPos;
            towerCollider.size = towerSize;
            towerCollider.tag = aot::physics::ColliderTag::Tower |
                                aot::physics::ColliderTag::Grappleable;
            towerTransform.SetPosition(towerPos.x, towerPos.y, towerPos.z);
        }
    }

}  // namespace aot::plugin::scene