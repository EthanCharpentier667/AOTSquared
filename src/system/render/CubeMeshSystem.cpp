/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CubeMeshSystem
*/

#include "CubeMeshSystem.hpp"

#include "../../maths/RaylibMaths.hpp"
#include "Object.hpp"

void CubeMeshSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto cubeView =
        registry.view<aot::geometry::CubeMesh, Object::Component::Transform>();

    cubeView.each([](auto &cube, auto &transform) {
        DrawCubeV(aot::RaylibMaths::toRayVector3(transform.GetPosition()),
                  cube.size, cube.color);
    });
}