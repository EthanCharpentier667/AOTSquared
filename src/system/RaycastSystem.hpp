/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RaycastSystem
*/

#pragma once

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

        direction = Vector3Normalize(direction);
        auto &registry = core->GetRegistry();

        // BoxColliders
        auto boxView = registry.view<aot::physics::BoxCollider>();
        for (auto entity : boxView) {
            auto &box = boxView.get<aot::physics::BoxCollider>(entity);

            if (!(static_cast<uint32_t>(box.tag) & layerMask))
                continue;

            Vector3 boxMin =
                Vector3Subtract(box.position, Vector3Scale(box.size, 0.5f));
            Vector3 boxMax =
                Vector3Add(box.position, Vector3Scale(box.size, 0.5f));

            float tmin = 0.0f, tmax = maxDistance;
            for (int i = 0; i < 3; i++) {
                float originAxis = 0.0f;
                float directionAxis = 0.0f;
                float boxMinAxis = 0.0f;
                float boxMaxAxis = 0.0f;

                if (i == 0) {
                    originAxis = origin.x;
                    directionAxis = direction.x;
                    boxMinAxis = boxMin.x;
                    boxMaxAxis = boxMax.x;
                } else if (i == 1) {
                    originAxis = origin.y;
                    directionAxis = direction.y;
                    boxMinAxis = boxMin.y;
                    boxMaxAxis = boxMax.y;
                } else {
                    originAxis = origin.z;
                    directionAxis = direction.z;
                    boxMinAxis = boxMin.z;
                    boxMaxAxis = boxMax.z;
                }

                float denom = directionAxis;
                if (fabsf(denom) < 0.0001f)
                    continue;
                float t1 = (boxMinAxis - originAxis) / denom;
                float t2 = (boxMaxAxis - originAxis) / denom;
                if (t1 > t2)
                    std::swap(t1, t2);
                tmin = std::max(tmin, t1);
                tmax = std::min(tmax, t2);
                if (tmin > tmax)
                    break;
            }

            if (tmin > 0.0001f && tmin < closestHit.distance) {
                closestHit.hit = true;
                closestHit.distance = tmin;
                closestHit.point =
                    Vector3Add(origin, Vector3Scale(direction, tmin));
                closestHit.normal = (Vector3){0.0f, 1.0f, 0.0f};
            }
        }

        // SphereColliders
        auto sphereView = registry.view<aot::physics::SphereCollider>();
        for (auto entity : sphereView) {
            auto &sphere = sphereView.get<aot::physics::SphereCollider>(entity);

            if (!(static_cast<uint32_t>(sphere.tag) & layerMask))
                continue;

            Vector3 oc = Vector3Subtract(origin, sphere.position);
            float b = Vector3DotProduct(oc, direction);
            float c = Vector3DotProduct(oc, oc) - sphere.radius * sphere.radius;
            float discriminant = b * b - c;

            if (discriminant >= 0.0f) {
                float t = -b - sqrtf(discriminant);
                if (t > 0.0001f && t < closestHit.distance) {
                    closestHit.hit = true;
                    closestHit.distance = t;
                    closestHit.point =
                        Vector3Add(origin, Vector3Scale(direction, t));
                    closestHit.normal = Vector3Normalize(
                        Vector3Subtract(closestHit.point, sphere.position));
                }
            }
        }

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
