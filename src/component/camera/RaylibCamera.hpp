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
        Vector2 lookRotation = {0.0f, 0.0f};
        Vector2 lean = {0.0f, 0.0f};
        float headLerp = PLAYER_EYE_HEIGHT;
        float walkLerp = 0.0f;
        float headTimer = 0.0f;
        Vector2 sensitivity = {0.005f, 0.005f};
        float tpsDistance = 5.0f;
    };
}  // namespace aot::camera

#endif /* !RAYLIBCAMERA_HPP_ */
