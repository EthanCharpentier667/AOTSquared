/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** SphereMeshSystem
*/

#include "SphereMeshSystem.hpp"

#include "../../maths/RaylibMaths.hpp"
#include "Object.hpp"

void SphereMeshSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto sphereView =
        registry
            .view<aot::geometry::SphereMesh, Object::Component::Transform>();

    sphereView.each([](auto &sphere, auto &transform) {
        DrawSphereEx(aot::RaylibMaths::toRayVector3(transform.GetPosition()),
                     sphere.radius, static_cast<int>(sphere.rings),
                     static_cast<int>(sphere.slices), sphere.color);
    });
}