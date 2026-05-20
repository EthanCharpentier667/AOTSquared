/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** MonoBehivor
*/

#pragma once

#include "../component/AMonoBehavior.hpp"
#include "Raylib.hpp"

namespace aot::plugin {
    class MonoBehivor : public Engine::APlugin {
      public:
        using APlugin::APlugin;
        ~MonoBehivor() override = default;

        void Bind(void) override {
            RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
                auto behaviors = aot::AMonoBehavior::GetInstances();
                for (auto *behavior : behaviors) {
                    behavior->EnsureStarted(core);
                    behavior->Update(core);
                    behavior->ProcessInvokeQueue();
                }
            });

            RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(
                [](Engine::Core &core) {
                    auto behaviors = aot::AMonoBehavior::GetInstances();
                    for (auto *behavior : behaviors) {
                        behavior->EnsureStarted(core);
                        behavior->FixedUpdate(core);
                    }
                });

            RegisterSystems<Engine::Scheduler::Shutdown>(
                [](Engine::Core &core) {
                    auto behaviors = aot::AMonoBehavior::GetInstances();
                    for (auto *behavior : behaviors) {
                        behavior->Stop(core);
                    }
                });
        }

      protected:
      private:
    };
}  // namespace aot::plugin
