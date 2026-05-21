/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** SphereMesh
*/

#pragma once

#include <cstdint>

#include "../../Raylib.hpp"

namespace aot::geometry {
    struct SphereMesh {
        float radius = 0.5f;
        std::uint32_t rings = 16u;
        std::uint32_t slices = 32u;
        Color color = WHITE;
    };
}  // namespace aot::geometry