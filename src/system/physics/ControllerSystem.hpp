/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ControllerSystem
*/

#pragma once

#include "../../component/character/Controller.hpp"
#include "Raylib.hpp"

namespace aot::character {
    struct Rigidbody;
}

namespace aot::physics {

    void RegisterControllerSystems(Engine::Core &core);

    void UpdateControllers(Engine::Core &core);

    Vector3 CalculateJumpVelocity(Vector3 startPoint, Vector3 endPoint,
                                  float trajectoryHeight);

    void StartGrapple(aot::character::Rigidbody *rigidBody,
                      Vector3 targetPosition, float trajectoryHeight);

    void StopGrapple(aot::character::Rigidbody *rigidBody,
                     float momentumMultiplier = 1.0f);

}  // namespace aot::physics