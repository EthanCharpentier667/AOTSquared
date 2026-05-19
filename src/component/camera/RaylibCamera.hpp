/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RaylibCamera
*/

#ifndef RAYLIBCAMERA_HPP_
#define RAYLIBCAMERA_HPP_

#include "Raylib.hpp"

namespace aot::camera {
    struct RaylibCamera {
        RaylibCamera() = default;

        RaylibCamera(Vector3 position, Vector3 target, Vector3 up, float fovy,
                     int projection)
            : camera{.position = position,
                     .target = target,
                     .up = up,
                     .fovy = fovy,
                     .projection = projection} {
        }

        RaylibCamera(const Camera &c) : camera(c) {
        }

        Camera camera{0};
    };
}  // namespace aot::camera

#endif /* !RAYLIBCAMERA_HPP_ */
