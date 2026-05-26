/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Raycast
*/

#ifndef RAYCAST_HPP_
#define RAYCAST_HPP_

#include "Collider.hpp"
#include "Raylib.hpp"

struct EngineCore;

namespace aot::physics {
    struct RaycastHit {
        bool hit = false;
        Vector3 point = {0.0f, 0.0f, 0.0f};
        Vector3 normal = {0.0f, 0.0f, 0.0f};
        float distance = 0.0f;
    };

    inline RaycastHit Raycast(Vector3 origin, Vector3 direction,
                              float maxDistance, uint32_t layerMask,
                              Engine::Core *core = nullptr) {
        RaycastHit closestHit;
        closestHit.distance = maxDistance;

        if (!core)
            return closestHit;

        if (Vector3LengthSqr(direction) <= 0.000001f)
            return closestHit;

        Ray ray = {origin, Vector3Normalize(direction)};
        auto &registry = core->GetRegistry();

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
}  // namespace aot::physics

#endif /* !RAYCAST_HPP_ */
