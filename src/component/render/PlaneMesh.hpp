/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** PlaneMesh
*/

#pragma once

#include <cstdint>

#include "../../Raylib.hpp"

namespace aot::geometry {
    struct PlaneMesh {
        float width = 1.0f;
        float depth = 1.0f;
        std::uint32_t subdivisionsX = 1u;
        std::uint32_t subdivisionsZ = 1u;
        Color color = WHITE;
    };
}  // namespace aot::geometry