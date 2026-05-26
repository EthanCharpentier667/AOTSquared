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
#include "../component/render/CubeMesh.hpp"
#include "../component/render/PlaneMesh.hpp"
#include "../component/render/SphereMesh.hpp"
#include "../plugin/render/Renders.hpp"
#include "../system/LineRendererSystem.hpp"
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
        void _onCreate(Engine::Core &core) final;

        void _onDestroy(Engine::Core &core) final;

      private:
        void SetupGround(Engine::Core &core);
        void SetupSun(Engine::Core &core);
        void SetupTowers(Engine::Core &core);

        Engine::Entity SetupCamera(Engine::Core &core, Engine::Entity player);
        Engine::Entity SetupPlayer(Engine::Core &core);
    };
}  // namespace aot::plugin::scene

#endif /* !TESTSCENE_HPP_ */
