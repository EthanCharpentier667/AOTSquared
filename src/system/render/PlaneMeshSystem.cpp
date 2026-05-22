/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** PlaneMeshSystem
*/

#include "PlaneMeshSystem.hpp"

#include "../../maths/RaylibMaths.hpp"
#include "Object.hpp"

void PlaneMeshSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto planeView =
        registry.view<aot::geometry::PlaneMesh, Object::Component::Transform>();

    planeView.each([](auto &plane, auto &transform) {
        DrawPlane(aot::RaylibMaths::toRayVector3(transform.GetPosition()),
                  {plane.width, plane.depth}, plane.color);
    });
}