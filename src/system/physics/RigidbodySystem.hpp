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

}  // namespace aot::physics