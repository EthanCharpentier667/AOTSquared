#pragma once

#include "Raylib.hpp"

namespace aot::character {
    struct Controller {
        Vector3 dir = {0};
        Vector3 dir3D = {0};
        bool crouching = false;
        bool jumpPressed = false;
    };
}  // namespace aot::character
