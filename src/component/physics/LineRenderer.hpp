/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** LineRenderer
*/

#ifndef LINERENDERER_HPP_
#define LINERENDERER_HPP_

#include "../../Raylib.hpp"

namespace aot::physics {
    struct LineRenderer {
        bool enabled = false;
        Vector3 startPoint = {0.0f, 0.0f, 0.0f};
        Vector3 endPoint = {0.0f, 0.0f, 0.0f};
        float width = 1.0f;
        float height = 1.0f;
        Color color = BLACK;
    };
}  // namespace aot::physics

#endif /* !LINERENDERER_HPP_ */