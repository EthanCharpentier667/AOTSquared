/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ColliderGizmoSystem
*/

#include "ColliderGizmoSystem.hpp"

#include "../../component/physics/ColliderUtils.hpp"
#include "Object.hpp"
#include "Raylib.hpp"

namespace aot::physics {
    void ColliderGizmoSystem(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto boxView =
            registry.view<BoxCollider, Object::Component::Transform>();
        auto sphereView =
            registry.view<SphereCollider, Object::Component::Transform>();
        auto capsuleView =
            registry.view<CapsuleCollider, Object::Component::Transform>();

        boxView.each([](auto &collider, auto &transform) {
            if (collider.activeGizmo) {
                collider.position = aot::physics::GetColliderWorldPosition(
                    transform, collider.offset);
                DrawBoundingBox(collider.GetBoundingBox(), collider.gizmoColor);
            }
        });

        sphereView.each([](auto &collider, auto &transform) {
            if (collider.activeGizmo) {
                collider.position = aot::physics::GetColliderWorldPosition(
                    transform, collider.offset);
                DrawSphereWires(collider.position, collider.radius, 4, 4,
                                collider.gizmoColor);
            }
        });

        capsuleView.each([](auto &collider, auto &transform) {
            if (!collider.activeGizmo)
                return;

            collider.position = aot::physics::GetColliderWorldPosition(
                transform, collider.offset);

            Vector3 bottom = collider.GetBottomSphereCenter();
            Vector3 top = collider.GetTopSphereCenter();

            DrawCapsuleWires(bottom, top, collider.radius, 8, 8,
                             collider.gizmoColor);
        });
    }
}  // namespace aot::physics