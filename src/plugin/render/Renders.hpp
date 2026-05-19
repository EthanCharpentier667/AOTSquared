/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Render
*/

#pragma once

#include "Engine.hpp"

namespace aot::plugin::render {
    class PreRender : public Engine::Scheduler::Update {
      public:
        using Engine::Scheduler::Update::Update;
    };

    class Render : public Engine::Scheduler::Update {
      public:
        using Engine::Scheduler::Update::Update;
    };

    class PostRender : public Engine::Scheduler::Update {
      public:
        using Engine::Scheduler::Update::Update;
    };
}  // namespace aot::plugin::render
