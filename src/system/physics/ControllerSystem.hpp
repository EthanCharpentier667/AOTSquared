/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ControllerSystem
*/

#pragma once

#include "../../component/character/Controller.hpp"
#include "Raylib.hpp"

namespace aot::physics {

    void RegisterControllerSystems(Engine::Core &core);

    void UpdateControllers(Engine::Core &core);

}  // namespace aot::physics