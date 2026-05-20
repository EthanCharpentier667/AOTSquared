/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** AMonoBehaviorInvokeSystem
*/

#pragma once

#include "../component/AMonoBehavior.hpp"
#include "Engine.hpp"
#include "Raylib.hpp"

namespace aot {
    inline void AMonoBehaviorInvokeSystem(Engine::Core &core) {
        (void)core;
        float delta = GetFrameTime();

        for (auto *behavior : AMonoBehavior::instances) {
            behavior->ProcessInvokeQueue(delta);
        }
    }
}  // namespace aot
