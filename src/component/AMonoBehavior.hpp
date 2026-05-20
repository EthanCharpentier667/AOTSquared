/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** AMonoBehavior
*/

#ifndef AMONOBEHAVIOR_HPP_
#define AMONOBEHAVIOR_HPP_

#include <algorithm>
#include <functional>
#include <vector>

#include "IMonoBehavior.hpp"
#include "Raylib.hpp"

namespace aot {
    class AMonoBehavior : public IMonoBehavior {
      public:
        AMonoBehavior() {
            GetInstances().push_back(this);
        };
        ~AMonoBehavior() override {
            auto &instances = GetInstances();
            instances.erase(
                std::remove(instances.begin(), instances.end(), this),
                instances.end());
        }

        static std::vector<AMonoBehavior *> &GetInstances() {
            static std::vector<AMonoBehavior *> instances;
            return instances;
        }

        void EnsureStarted(Engine::Core &core) {
            if (started)
                return;

            Start(core);
            started = true;
        }

      protected:
        template <typename T>
        void ResolveSelf(Engine::Core &core) {
            auto &registry = core.GetRegistry();
            auto view = registry.view<T>();

            for (auto e : view) {
                if (&view.template get<T>(e) == this) {
                    self = e;
                    return;
                }
            }
            Log::Error("MonoBehavior instance not found in registry");
        }

      public:
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

        void Invoke(float delay, std::function<void(void)> callback) {
            invokeQueue.push_back({delay, callback});
        }

        void ProcessInvokeQueue() {
            float delta = GetFrameTime();
            std::vector<std::function<void(void)>> callbacksToRun;

            for (auto it = invokeQueue.begin(); it != invokeQueue.end();) {
                it->timer -= delta;
                if (it->timer <= 0.0f) {
                    callbacksToRun.push_back(std::move(it->callback));
                    it = invokeQueue.erase(it);
                } else {
                    ++it;
                }
            }

            for (auto &callback : callbacksToRun) {
                callback();
            }
        }

      protected:
        struct InvokeEvent {
            float timer;
            std::function<void(void)> callback;
        };

        std::vector<InvokeEvent> invokeQueue;
        Engine::Id self;
        bool started = false;
    };
}  // namespace aot

#endif /* !AMONOBEHAVIOR_HPP_ */
