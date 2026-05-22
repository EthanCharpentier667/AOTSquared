/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RaycastSystem
*/

#ifndef RAYCASTSYSTEM_HPP_
#define RAYCASTSYSTEM_HPP_

#include "../component/physics/Raycast.hpp"
#include "Raylib.hpp"
#include "component/physics/Collider.hpp"

namespace aot::physics {
    inline RaycastHit RaycastAgainstColliders(Vector3 origin, Vector3 direction,
                                              float maxDistance,
                                              uint32_t layerMask,
                                              Engine::Core *core = nullptr) {
        RaycastHit closestHit;
        closestHit.distance = maxDistance;

        if (!core)
            return closestHit;

        if (Vector3LengthSqr(direction) <= 0.000001f)
            return closestHit;

        Ray ray = {origin, Vector3Normalize(direction)};
        auto &registry = core->GetRegistry();

        // BoxColliders
        auto boxView = registry.view<aot::physics::BoxCollider>();
        boxView.each([&](auto &box) {
            if (!(static_cast<uint32_t>(box.tag) & layerMask))
                return;

            RayCollision collision = box.GetCollision(ray);

            if (collision.hit && collision.distance <= maxDistance &&
                collision.distance < closestHit.distance) {
                closestHit.hit = true;
                closestHit.distance = collision.distance;
                closestHit.point = collision.point;
                closestHit.normal = collision.normal;
            }
        });

        // SphereColliders
        auto sphereView = registry.view<aot::physics::SphereCollider>();
        sphereView.each([&](auto &sphere) {
            if (!(static_cast<uint32_t>(sphere.tag) & layerMask))
                return;

            RayCollision collision = sphere.GetCollision(ray);

            if (collision.hit && collision.distance <= maxDistance &&
                collision.distance < closestHit.distance) {
                closestHit.hit = true;
                closestHit.distance = collision.distance;
                closestHit.point = collision.point;
                closestHit.normal = collision.normal;
            }
        });

        return closestHit;
    }

    inline RaycastHit SimpleRaycast(Vector3 origin, Vector3 direction,
                                    float maxDistance) {
        RaycastHit hit;

        direction = Vector3Normalize(direction);

        if (direction.y < 0.0f) {
            float t = -origin.y / direction.y;
            if (t > 0.0f && t < maxDistance) {
                Vector3 hitPoint =
                    Vector3Add(origin, Vector3Scale(direction, t));
                hit.hit = true;
                hit.point = hitPoint;
                hit.normal = (Vector3){0.0f, 1.0f, 0.0f};
                hit.distance = t;
            }
        }

        return hit;
    }
}  // namespace aot::physics

void RaycastSystem(Engine::Core &core);

#endif /* RAYCASTSYSTEM_HPP_ */
