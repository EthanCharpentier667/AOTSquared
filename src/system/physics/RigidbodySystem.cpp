/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RigidbodySystem
*/

#include "RigidbodySystem.hpp"

#include <algorithm>
#include <cmath>

#include "Object.hpp"
#include "component/character/Controller.hpp"
#include "component/character/Rigidbody.hpp"

namespace aot::physics {

    Vector3 CalculateJumpVelocity(Vector3 startPoint, Vector3 endPoint,
                                  float trajectoryHeight) {
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

    void StartGrapple(aot::character::Rigidbody &rigidBody,
                      Vector3 targetPosition, float trajectoryHeight) {
        rigidBody.activeGrapple = true;
        rigidBody.grappleTarget = targetPosition;
        rigidBody.velocity = CalculateJumpVelocity(
            rigidBody.position, targetPosition, trajectoryHeight);
        rigidBody.isGrounded = false;
        rigidBody.state = aot::character::MouvementState::Air;
    }

    void StopGrapple(aot::character::Rigidbody &rigidBody) {
        rigidBody.activeGrapple = false;
        rigidBody.grappleTarget = rigidBody.position;
        rigidBody.velocity = {0.0f, 0.0f, 0.0f};
        rigidBody.state = rigidBody.isGrounded
                              ? aot::character::MouvementState::Idle
                              : aot::character::MouvementState::Air;
    }

    static void UpdateRigidbody(aot::character::Rigidbody &rigidBody,
                                aot::character::Controller &controller,
                                Object::Component::Transform &transform) {
        float delta = GetFrameTime();

        if (rigidBody.activeGrapple) {
            Vector3 toTarget = {
                rigidBody.grappleTarget.x - rigidBody.position.x,
                rigidBody.grappleTarget.y - rigidBody.position.y,
                rigidBody.grappleTarget.z - rigidBody.position.z,
            };

            float distance = Vector3Length(toTarget);
            if (distance <= rigidBody.grappleStopDistance) {
                rigidBody.activeGrapple = false;
                rigidBody.velocity = {0.0f, 0.0f, 0.0f};
                return;
            }

            Vector3 pullDirection = Vector3Normalize(toTarget);
            rigidBody.velocity.x +=
                pullDirection.x * rigidBody.grapplePullStrength * delta;
            rigidBody.velocity.y +=
                pullDirection.y * rigidBody.grapplePullStrength * delta;
            rigidBody.velocity.z +=
                pullDirection.z * rigidBody.grapplePullStrength * delta;

            float currentSpeed = Vector3Length(rigidBody.velocity);
            if (currentSpeed > rigidBody.grappleMaxSpeed) {
                rigidBody.velocity =
                    Vector3Scale(Vector3Normalize(rigidBody.velocity),
                                 rigidBody.grappleMaxSpeed);
            }

            rigidBody.position.x += rigidBody.velocity.x * delta;
            rigidBody.position.y += rigidBody.velocity.y * delta;
            rigidBody.position.z += rigidBody.velocity.z * delta;

            if (rigidBody.position.y <= 0.0f) {
                rigidBody.position.y = 0.0f;
                rigidBody.velocity.y = 0.0f;
                rigidBody.isGrounded = true;
            }

            transform.SetRotation(aot::RaylibMaths::toGlmQuaternion(
                {controller.lookRotation.y, controller.lookRotation.x, 0.0f,
                 0.0f}));
            transform.SetPosition(aot::RaylibMaths::toGlmVector3(
                {rigidBody.position.x, rigidBody.position.y,
                 rigidBody.position.z}));
            return;
        }

        if (!rigidBody.isGrounded) {
            rigidBody.state = aot::character::MouvementState::Air;
            rigidBody.velocity.y -= GRAVITY * delta;
        }

        if (rigidBody.isGrounded && controller.jumpPressed) {
            rigidBody.state = aot::character::MouvementState::Air;
            rigidBody.velocity.y = JUMP_FORCE;
            rigidBody.isGrounded = false;
        }

        float decel = (rigidBody.isGrounded ? FRICTION : AIR_DRAG);
        Vector3 hvel = {rigidBody.velocity.x * decel, 0.0f,
                        rigidBody.velocity.z * decel};

        float hvelLength = Vector3Length(hvel);
        if (hvelLength < (MAX_SPEED * 0.01f))
            hvel = (Vector3){0};

        float speed = Vector3DotProduct(hvel, controller.dir);

        if (controller.crouching) {
            rigidBody.state = aot::character::MouvementState::Crouching;
        } else if (controller.dir.x != 0.0f || controller.dir.z != 0.0f) {
            rigidBody.state = aot::character::MouvementState::Running;
        } else {
            rigidBody.state = aot::character::MouvementState::Walking;
        }

        float maxSpeed = (controller.crouching ? CROUCH_SPEED : MAX_SPEED);
        float accel = Clamp(maxSpeed - speed, 0.f, MAX_ACCEL * delta);
        hvel.x += controller.dir.x * accel;
        hvel.z += controller.dir.z * accel;

        rigidBody.velocity.x = hvel.x;
        rigidBody.velocity.z = hvel.z;

        rigidBody.position.x += rigidBody.velocity.x * delta;
        rigidBody.position.y += rigidBody.velocity.y * delta;
        rigidBody.position.z += rigidBody.velocity.z * delta;

        if (rigidBody.position.y <= 0.0f) {
            rigidBody.position.y = 0.0f;
            rigidBody.velocity.y = 0.0f;
            rigidBody.isGrounded = true;
        }

        transform.SetRotation(aot::RaylibMaths::toGlmQuaternion(
            {controller.lookRotation.y, controller.lookRotation.x, 0.0f,
             0.0f}));
        transform.SetPosition(aot::RaylibMaths::toGlmVector3(
            {rigidBody.position.x, rigidBody.position.y,
             rigidBody.position.z}));
    }

    void UpdateRigidbodies(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto view =
            registry.view<aot::character::Controller, aot::character::Rigidbody,
                          Object::Component::Transform>();

        view.each([](auto &controller, auto &rigidBody, auto &transform) {
            UpdateRigidbody(rigidBody, controller, transform);
        });
    }

    void RegisterRigidbodySystems(Engine::Core &core) {
        core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
            UpdateRigidbodies);
    }

}  // namespace aot::physics