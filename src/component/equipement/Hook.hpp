/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Hook
*/

#pragma once

#include <optional>

#include "Raylib.hpp"

namespace aot::gear {
    struct Hook {
        std::optional<Engine::Entity> anchor = std::nullopt;
        Vector3 grapplePoint = {0.0f, 0.0f, 0.0f};
        float maxGrapDistance = 50.0f;
        float grappleTimeDelay = 0.1f;
        float grapplingCd = 1.0f;
        float grapplingCdTimer = 0.0f;
        float grappleDelayTimer = 0.0f;
        bool grappling = false;
        float overshootYAxis = 5.0f;
    };
}  // namespace aot::gear
