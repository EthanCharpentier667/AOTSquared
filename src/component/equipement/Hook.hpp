/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Hook
*/

#pragma once

#include <optional>

#include "../physics/Raycast.hpp"
#include "Raylib.hpp"

namespace aot::gear {
    struct Hook {
        Vector3 grapplePoint = {0.0f, 0.0f, 0.0f};
        aot::physics::RaycastHit grappleHit;
        float maxGrapDistance = 100.0f;
        float grappleTimeDelay = 0.1f;
        float grapplingCd = 0.1f;
        float grapplingCdTimer = 0.0f;
        float grappleDelayTimer = 0.0f;
        bool grappling = false;
        float overshootYAxis = 1.0f;
        int key = KEY_E;
    };
}  // namespace aot::gear
