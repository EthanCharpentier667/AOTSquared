#pragma once

#include "../AMonoBehavior.hpp"

namespace aot::character {

    class Controller : public AMonoBehavior {
      public:
        Controller();
        ~Controller() override = default;

        void Start(Engine::Core &core) override;
        void Update(Engine::Core &core) override;
        void FixedUpdate(Engine::Core &core) override;
        void Stop(Engine::Core &core) override;

        float headLerp = STAND_HEIGHT;
        float walkLerp = 0.0f;
        float headTimer = 0.0f;
        Vector2 lookRotation = {0};
        Vector2 lean = {0};
        Vector3 dir = {0};
        bool crouching = false;
        bool jumpPressed = false;
        Vector2 sensitivity = {0.001f, 0.001f};
    };
}  // namespace aot::character
