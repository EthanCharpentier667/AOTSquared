/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CubeMeshSystem
*/

#include "CubeMeshSystem.hpp"

#include "../component/camera/RaylibCamera.hpp"
#include "../maths/RaylibMaths.hpp"
#include "Object.hpp"

void CubeMeshSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto cameraView = registry.view<aot::camera::RaylibCamera>();
    auto cubeView =
        registry.view<aot::geometry::CubeMesh, Object::Component::Transform>();

    for (auto cameraEntity : cameraView) {
        auto &camera =
            cameraView.get<aot::camera::RaylibCamera>(cameraEntity).camera;
        BeginMode3D(camera);
        for (auto entity : cubeView) {
            auto &cube = cubeView.get<aot::geometry::CubeMesh>(entity);
            auto &transform =
                cubeView.get<Object::Component::Transform>(entity);
            DrawCubeV(aot::RaylibMaths::toRayVector3(transform.GetPosition()),
                      cube.size, cube.color);
        }
        EndMode3D();
    }
}