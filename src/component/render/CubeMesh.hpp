/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CubeMesh
*/

#pragma once

#include "../../Raylib.hpp"

namespace aot::geometry {
    struct CubeMesh {
        Vector3 size = {1.0f, 1.0f, 1.0f};
        Color color = WHITE;
    };
}  // namespace aot::geometry