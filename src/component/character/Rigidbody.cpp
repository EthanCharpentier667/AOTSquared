#include "Rigidbody.hpp"

#include <algorithm>
#include <cmath>

#include "Controller.hpp"
#include "Object.hpp"

namespace aot::character {

    Rigidbody::Rigidbody() : AMonoBehavior() {
    }

    void Rigidbody::Start(Engine::Core &core) {
        ResolveSelf<Rigidbody>(core);
        auto &registry = core.GetRegistry();
        _controller = registry.try_get<aot::character::Controller>(self);
        _transform = registry.try_get<Object::Component::Transform>(self);
    }

    void Rigidbody::Update(Engine::Core &core) {
        (void)core;
    }

    void Rigidbody::FixedUpdate(Engine::Core &core) {
        if (!_controller || !_transform)
            return;

        float delta = GetFrameTime();

        if (activeGrapple) {
            Vector3 toTarget = {
                grappleTarget.x - position.x,
                grappleTarget.y - position.y,
                grappleTarget.z - position.z,
            };

            float distance = Vector3Length(toTarget);
            if (distance <= grappleStopDistance) {
                activeGrapple = false;
                velocity = {0.0f, 0.0f, 0.0f};
                return;
            }

            Vector3 pullDirection = Vector3Normalize(toTarget);
            velocity.x += pullDirection.x * grapplePullStrength * delta;
            velocity.y += pullDirection.y * grapplePullStrength * delta;
            velocity.z += pullDirection.z * grapplePullStrength * delta;

            float currentSpeed = Vector3Length(velocity);
            if (currentSpeed > grappleMaxSpeed) {
                velocity =
                    Vector3Scale(Vector3Normalize(velocity), grappleMaxSpeed);
            }

            position.x += velocity.x * delta;
            position.y += velocity.y * delta;
            position.z += velocity.z * delta;

            if (position.y <= 0.0f) {
                position.y = 0.0f;
                velocity.y = 0.0f;
                isGrounded = true;
            }

            _transform->SetRotation(aot::RaylibMaths::toGlmQuaternion(
                {_controller->lookRotation.y, _controller->lookRotation.x, 0.0f,
                 0.0f}));
            _transform->SetPosition(aot::RaylibMaths::toGlmVector3(
                {position.x, position.y, position.z}));
            return;
        }

        if (!isGrounded) {
            state = aot::character::MouvementState::Air;
            velocity.y -= GRAVITY * delta;
        }

        if (isGrounded && _controller->jumpPressed) {
            state = aot::character::MouvementState::Air;
            velocity.y = JUMP_FORCE;
            isGrounded = false;
        }

        float decel = (isGrounded ? FRICTION : AIR_DRAG);
        Vector3 hvel = (Vector3){velocity.x * decel, 0.0f, velocity.z * decel};

        float hvelLength = Vector3Length(hvel);
        if (hvelLength < (MAX_SPEED * 0.01f))
            hvel = (Vector3){0};

        float speed = Vector3DotProduct(hvel, _controller->dir);

        if (_controller->crouching) {
            state = aot::character::MouvementState::Crouching;
        } else if (_controller->dir.x != 0.0f || _controller->dir.z != 0.0f) {
            state = aot::character::MouvementState::Running;
        } else {
            state = aot::character::MouvementState::Walking;
        }

        float maxSpeed = (_controller->crouching ? CROUCH_SPEED : MAX_SPEED);
        float accel = Clamp(maxSpeed - speed, 0.f, MAX_ACCEL * delta);
        hvel.x += _controller->dir.x * accel;
        hvel.z += _controller->dir.z * accel;

        velocity.x = hvel.x;
        velocity.z = hvel.z;

        position.x += velocity.x * delta;
        position.y += velocity.y * delta;
        position.z += velocity.z * delta;

        if (position.y <= 0.0f) {
            position.y = 0.0f;
            velocity.y = 0.0f;
            isGrounded = true;
        }

        _transform->SetRotation(aot::RaylibMaths::toGlmQuaternion(
            {_controller->lookRotation.y, _controller->lookRotation.x, 0.0f,
             0.0f}));
        _transform->SetPosition(aot::RaylibMaths::toGlmVector3(
            {position.x, position.y, position.z}));
    }

    void Rigidbody::Stop(Engine::Core &core) {
        (void)core;
    }

    Vector3 Rigidbody::CalculateJumpVelocity(Vector3 startPoint,
                                             Vector3 endPoint,
                                             float trajectoryHeight) const {
        float gravity = GRAVITY;
        float jumpHeight = std::max(trajectoryHeight, 0.001f);
        float displacementY = endPoint.y - startPoint.y;
        Vector3 displacementXZ = {
            endPoint.x - startPoint.x,
            0.0f,
            endPoint.z - startPoint.z,
        };

        float timeToApex = std::sqrt(2.0f * jumpHeight / gravity);
        float heightToTarget = std::max(jumpHeight - displacementY, 0.001f);
        float timeFromApex = std::sqrt(2.0f * heightToTarget / gravity);
        float totalTime = std::max(timeToApex + timeFromApex, 0.001f);

        return {
            displacementXZ.x / totalTime,
            std::sqrt(2.0f * gravity * jumpHeight),
            displacementXZ.z / totalTime,
        };
    }

    void Rigidbody::JumpToPosition(Vector3 targetPosition,
                                   float trajectoryHeight) {
        activeGrapple = true;
        grappleTarget = targetPosition;
        velocity =
            CalculateJumpVelocity(position, targetPosition, trajectoryHeight);
        isGrounded = false;
        state = MouvementState::Air;
    }

    void Rigidbody::StopGrapple() {
        activeGrapple = false;
        grappleTarget = position;
        velocity = {0.0f, 0.0f, 0.0f};
        state = isGrounded ? MouvementState::Idle : MouvementState::Air;
    }

}  // namespace aot::character
