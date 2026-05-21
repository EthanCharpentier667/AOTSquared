/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ColliderGizmoSystem
*/

#include "ColliderGizmoSystem.hpp"

#include "../component/camera/RaylibCamera.hpp"
#include "Raylib.hpp"

namespace aot::physics {
    void ColliderGizmoSystem(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        auto boxView = registry.view<BoxCollider>();
        auto sphereView = registry.view<SphereCollider>();

        for (auto cameraEntity : cameraView) {
            auto &camera =
                cameraView.get<aot::camera::RaylibCamera>(cameraEntity).camera;

            BeginMode3D(camera);

            for (auto entity : boxView) {
                auto &collider = boxView.get<BoxCollider>(entity);
                if (collider.activeGizmo) {
                    DrawCubeWiresV(collider.position, collider.size, RED);
                }
            }

            for (auto entity : sphereView) {
                auto &collider = sphereView.get<SphereCollider>(entity);
                if (collider.activeGizmo) {
                    DrawSphereWires(collider.position, collider.radius, 16, 16,
                                    RED);
                }
            }

            EndMode3D();
        }
    }
}  // namespace aot::physics