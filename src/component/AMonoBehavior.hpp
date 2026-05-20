/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** AMonoBehavior
*/

#ifndef AMONOBEHAVIOR_HPP_
#define AMONOBEHAVIOR_HPP_

#include "IMonoBehavior.hpp"
#include "Raylib.hpp"

namespace aot {
    class AMonoBehavior : public IMonoBehavior {
      public:
        AMonoBehavior(Engine::Core &core) {
            core.RegisterSystem<Engine::Scheduler::Startup>(
                [this](Engine::Core &engineCore) { Start(engineCore); });
            core.RegisterSystem<Engine::Scheduler::Update>(
                [this](Engine::Core &engineCore) { Update(engineCore); });
            core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
                [this](Engine::Core &engineCore) { FixedUpdate(engineCore); });
            core.RegisterSystem<Engine::Scheduler::Shutdown>(
                [this](Engine::Core &engineCore) { Stop(engineCore); });
        };
        ~AMonoBehavior() override = default;

        void Start(Engine::Core &core) override {
            (void)core;
        }

        void Update(Engine::Core &core) override {
            (void)core;
        }

        void FixedUpdate(Engine::Core &core) override {
            (void)core;
        }

        void Stop(Engine::Core &core) override {
            (void)core;
        }
    };
}  // namespace aot

#endif /* !AMONOBEHAVIOR_HPP_ */
