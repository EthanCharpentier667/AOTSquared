#pragma once

#include "Raylib.hpp"

namespace aot::character {
    enum class MouvementState {
        Idle,
        Walking,
        Running,
        Crouching,
        Air,
    };

    struct Rigidbody {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 velocity = {0.0f, 0.0f, 0.0f};
        bool isGrounded = true;
        bool activeGrapple = false;
        MouvementState state = MouvementState::Idle;

        Vector3 grappleTarget = {0.0f, 0.0f, 0.0f};
        float grapplePullStrength = 60.0f;
        float grappleMaxSpeed = 55.0f;
        float grappleStopDistance = 1.5f;
    };
}  // namespace aot::character
