/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ColliderUtils
*/

#pragma once

#include "Collider.hpp"
#include "Object.hpp"
#include "Raylib.hpp"

namespace aot::physics {
    inline Vector3 GetColliderWorldPosition(
        const Object::Component::Transform &transform, Vector3 offset) {
        return Vector3Add(
            aot::RaylibMaths::toRayVector3(transform.GetPosition()), offset);
    }
}  // namespace aot::physics
