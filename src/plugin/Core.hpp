/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Core
*/

#pragma once

#include "Raylib.hpp"
#include "RaylibPlugin.hpp"
#include "render/Renders.hpp"

namespace aot::plugin {
    class Core : public Engine::APlugin {
      public:
        using APlugin::APlugin;
        ~Core() = default;

        void Bind(void) override {
            RequirePlugins<raylib::RaylibPlugin>();

            RegisterSystems<render::Render>(
                [](Engine::Core &) { ClearBackground(RAYWHITE); });
        }

      protected:
      private:
    };
}  // namespace aot::plugin
