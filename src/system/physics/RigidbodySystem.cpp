/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RigidbodySystem
*/

#include "RigidbodySystem.hpp"

#include "Object.hpp"
#include "component/camera/RaylibCamera.hpp"
#include "component/character/Rigidbody.hpp"
#include "system/physics/ControllerSystem.hpp"
#include "glm/gtc/quaternion.hpp"

namespace aot::physics {
    static void UpdateRigidbody(aot::character::Rigidbody &rigidBody,
                                float yaw,
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
                StopGrapple(&rigidBody, 1.0f);
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

            transform.SetRotation(
                glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f)));
            transform.SetPosition(aot::RaylibMaths::toGlmVector3(
                {rigidBody.position.x, rigidBody.position.y,
                 rigidBody.position.z}));
            return;
        }

        if (!rigidBody.isGrounded) {
            rigidBody.state = aot::character::MouvementState::Air;
            rigidBody.velocity.y -= GRAVITY * delta;
        }

        rigidBody.position.x += rigidBody.velocity.x * delta;
        rigidBody.position.y += rigidBody.velocity.y * delta;
        rigidBody.position.z += rigidBody.velocity.z * delta;

        transform.SetRotation(glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f)));
        transform.SetPosition(aot::RaylibMaths::toGlmVector3(
            {rigidBody.position.x, rigidBody.position.y,
             rigidBody.position.z}));
    }

    void UpdateRigidbodies(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto view = registry.view<aot::character::Rigidbody,
                                  Object::Component::Transform>();
        auto cameraView = registry.view<aot::camera::RaylibCamera>();

        float yaw = 0.0f;
        if (cameraView.begin() != cameraView.end()) {
            auto cameraEntity = *cameraView.begin();
            yaw = cameraView.get<aot::camera::RaylibCamera>(cameraEntity)
                      .lookRotation.x;
        }

        view.each([&](auto &rigidBody, auto &transform) {
            UpdateRigidbody(rigidBody, yaw, transform);
        });
    }

    void RegisterRigidbodySystems(Engine::Core &core) {
        core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(
            UpdateRigidbodies);
    }

}  // namespace aot::physics