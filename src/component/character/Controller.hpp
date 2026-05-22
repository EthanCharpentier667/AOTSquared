#pragma once

#include "Raylib.hpp"

namespace aot::character {
    struct Controller {
        float headLerp = STAND_HEIGHT;
        float walkLerp = 0.0f;
        float headTimer = 0.0f;
        Vector2 lookRotation = {0};
        Vector2 lean = {0};
        Vector3 dir = {0};
        bool crouching = false;
        bool jumpPressed = false;
        Vector2 sensitivity = {0.001f, 0.001f};
    };
}  // namespace aot::character
