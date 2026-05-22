/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RigidbodySystem
*/

#pragma once

#include "Raylib.hpp"

namespace aot::character {
    struct Rigidbody;
}

namespace aot::physics {

    void RegisterRigidbodySystems(Engine::Core &core);

    void UpdateRigidbodies(Engine::Core &core);

    Vector3 CalculateJumpVelocity(Vector3 startPoint, Vector3 endPoint,
                                  float trajectoryHeight);

    void StartGrapple(aot::character::Rigidbody &rigidBody,
                      Vector3 targetPosition, float trajectoryHeight);

    void StopGrapple(aot::character::Rigidbody &rigidBody);

}  // namespace aot::physics