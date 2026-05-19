/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RaylibPlugin
*/

#pragma once

#include "Raylib.hpp"

namespace aot::plugin::raylib {
    class RaylibPlugin : public Engine::APlugin {
      public:
        using APlugin::APlugin;
        ~RaylibPlugin() = default;

        void Bind(void) override;

      protected:
      private:
    };
}  // namespace aot::plugin::raylib
