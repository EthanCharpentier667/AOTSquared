/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Collider
*/

#ifndef COLLIDER_HPP_
#define COLLIDER_HPP_

#include "Raylib.hpp"

namespace aot::physics {

    enum class ColliderTag : uint32_t {
        None = 0,
        Ground = 1 << 0,       // Floor
        Tower = 1 << 1,        // Fixed Objects
        Enemy = 1 << 2,        // Ennemies
        Pickup = 1 << 3,       // Items to pickup
        Player = 1 << 4,       // Player
        Grappleable = 1 << 5,  // Objects on which to grapple
    };

    inline constexpr ColliderTag operator|(ColliderTag lhs, ColliderTag rhs) {
        return static_cast<ColliderTag>(static_cast<uint32_t>(lhs) |
                                        static_cast<uint32_t>(rhs));
    }

    inline constexpr ColliderTag operator&(ColliderTag lhs, ColliderTag rhs) {
        return static_cast<ColliderTag>(static_cast<uint32_t>(lhs) &
                                        static_cast<uint32_t>(rhs));
    }

    struct BoxCollider {
        BoxCollider(bool activeGizmo = false) : activeGizmo(activeGizmo) {
        }
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 size = {1.0f, 1.0f, 1.0f};
        ColliderTag tag = ColliderTag::None;
        uint32_t mask = static_cast<uint32_t>(ColliderTag::None);
        bool activeGizmo = false;

        [[nodiscard]] BoundingBox GetBoundingBox() const {
            const Vector3 halfSize = Vector3Scale(size, 0.5f);

            return {
                .min = Vector3Subtract(position, halfSize),
                .max = Vector3Add(position, halfSize),
            };
        }

        [[nodiscard]] RayCollision GetCollision(Ray ray) const {
            return GetRayCollisionBox(ray, GetBoundingBox());
        }
    };

    struct SphereCollider {
        SphereCollider(bool activeGizmo = false) : activeGizmo(activeGizmo) {
        }
        Vector3 position = {0.0f, 0.0f, 0.0f};
        float radius = 1.0f;s
        ColliderTag tag = ColliderTag::None;
        uint32_t mask = static_cast<uint32_t>(ColliderTag::None);
        bool activeGizmo = false;

        [[nodiscard]] RayCollision GetCollision(Ray ray) const {
            return GetRayCollisionSphere(ray, position, radius);
        }
    };

    struct Collider {
        Collider(bool activeGizmo = false) : activeGizmo(activeGizmo) {
        }
        BoxCollider *box = nullptr;
        SphereCollider *sphere = nullptr;
        bool activeGizmo = false;
    };
}  // namespace aot::physics

#endif /* !COLLIDER_HPP_ */
