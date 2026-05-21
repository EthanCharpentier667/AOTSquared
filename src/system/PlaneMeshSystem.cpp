/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** PlaneMeshSystem
*/

#include "PlaneMeshSystem.hpp"

#include "../component/camera/RaylibCamera.hpp"
#include "../maths/RaylibMaths.hpp"
#include "Object.hpp"

void PlaneMeshSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto cameraView = registry.view<aot::camera::RaylibCamera>();
    auto planeView =
        registry.view<aot::geometry::PlaneMesh, Object::Component::Transform>();

    for (auto cameraEntity : cameraView) {
        auto &camera =
            cameraView.get<aot::camera::RaylibCamera>(cameraEntity).camera;
        BeginMode3D(camera);
        for (auto entity : planeView) {
            auto &plane = planeView.get<aot::geometry::PlaneMesh>(entity);
            auto &transform =
                planeView.get<Object::Component::Transform>(entity);
            DrawPlane(aot::RaylibMaths::toRayVector3(transform.GetPosition()),
                      {plane.width, plane.depth}, plane.color);
        }
        EndMode3D();
    }
}