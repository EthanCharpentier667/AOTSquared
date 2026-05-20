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

namespace aot::physics {
    struct RaycastHit {
        bool hit = false;
        Vector3 point = {0.0f, 0.0f, 0.0f};
        Vector3 normal = {0.0f, 0.0f, 0.0f};
        float distance = 0.0f;
    };

    struct Raycast {
        Vector3 origin = {0.0f, 0.0f, 0.0f};
        Vector3 direction = {0.0f, 0.0f, 1.0f};
        float maxDistance = 100.0f;
        bool active = false;
        uint32_t layerMask = static_cast<uint32_t>(ColliderTag::None);

        RaycastHit result;
    };
}  // namespace aot::physics

#endif /* !RAYCAST_HPP_ */
