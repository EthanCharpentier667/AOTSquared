#pragma once

#include "Raylib.hpp"

namespace aot::character {
    struct Controller {
        Vector3 dir = {0};
        Vector3 dir3D = {0};
        Vector3 lookDir = {0.0f, 0.0f, 1.0f};
        bool crouching = false;
        bool jumpPressed = false;
        bool doubleSpacePressed = false;
        float lastSpacePressTime = -1.0f;
    };
}  // namespace aot::character
