/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** LineRendererSystem
*/

#pragma once

#include "../component/physics/LineRenderer.hpp"
#include "Raylib.hpp"

namespace aot::physics {
    inline void DrawThickLine3D(Vector3 start, Vector3 end, float radius,
                                Color color) {
        Vector3 diff = Vector3Subtract(end, start);
        float length = Vector3Length(diff);

        if (length <= 0.0001f)
            return;

        DrawCylinderEx(start, end, radius, radius, 12, color);
    }
}  // namespace aot::physics

void LineRendererSystem(Engine::Core &core);