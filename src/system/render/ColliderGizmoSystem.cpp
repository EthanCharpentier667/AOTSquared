/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ColliderGizmoSystem
*/

#include "ColliderGizmoSystem.hpp"

#include "Raylib.hpp"

namespace aot::physics {
    void ColliderGizmoSystem(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto boxView = registry.view<BoxCollider>();
        auto sphereView = registry.view<SphereCollider>();

        boxView.each([](auto &collider) {
            if (collider.activeGizmo) {
                DrawBoundingBox(collider.GetBoundingBox(), RED);
            }
        });

        sphereView.each([](auto &collider) {
            if (collider.activeGizmo) {
                DrawSphereWires(collider.position, collider.radius, 16, 16,
                                RED);
            }
        });
    }
}  // namespace aot::physics