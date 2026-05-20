#pragma once

#include <optional>

#include "../AMonoBehavior.hpp"
#include "Object.hpp"

namespace aot::character {
    enum class MouvementState {
        Idle,
        Walking,
        Running,
        Crouching,
        Air,
    };

    class Controller;

    class Rigidbody : public AMonoBehavior {
      public:
        Rigidbody();
        ~Rigidbody() override = default;

        void Start(Engine::Core &core) override;
        void Update(Engine::Core &core) override;
        void FixedUpdate(Engine::Core &core) override;
        void Stop(Engine::Core &core) override;

        void JumpToPosition(Vector3 targetPosition, float trajectoryHeight);

        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 velocity = {0.0f, 0.0f, 0.0f};
        bool isGrounded = true;
        bool enableMovementOnNextTouch = false;
        MouvementState state = MouvementState::Idle;

      private:
        Controller *_controller = nullptr;
        Object::Component::Transform *_transform = nullptr;

        Vector3 CalculateJumpVelocity(Vector3 startPoint, Vector3 endPoint,
                                      float trajectoryHeight) const;
    };
}  // namespace aot::character
