/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** SphereMeshSystem
*/

#include "SphereMeshSystem.hpp"

#include "../component/camera/RaylibCamera.hpp"
#include "../maths/RaylibMaths.hpp"
#include "Object.hpp"

void SphereMeshSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto cameraView = registry.view<aot::camera::RaylibCamera>();
    auto sphereView =
        registry
            .view<aot::geometry::SphereMesh, Object::Component::Transform>();

    for (auto cameraEntity : cameraView) {
        auto &camera =
            cameraView.get<aot::camera::RaylibCamera>(cameraEntity).camera;
        BeginMode3D(camera);
        for (auto entity : sphereView) {
            auto &sphere = sphereView.get<aot::geometry::SphereMesh>(entity);
            auto &transform =
                sphereView.get<Object::Component::Transform>(entity);
            DrawSphereEx(
                aot::RaylibMaths::toRayVector3(transform.GetPosition()),
                sphere.radius, static_cast<int>(sphere.rings),
                static_cast<int>(sphere.slices), sphere.color);
        }
        EndMode3D();
    }
}