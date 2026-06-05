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
#include "glm/gtc/quaternion.hpp"
#include "system/physics/ControllerSystem.hpp"

namespace aot::physics {
    static void UpdateRigidbody(aot::character::Rigidbody &rigidBody, float yaw,
                                Object::Component::Transform &transform) {
        float delta = GetFrameTime();

        if (rigidBody.activeGrapple) {
            rigidBody.velocity.y -= GRAVITY * 0.5f * delta;

            Vector3 toTarget = {
                rigidBody.grappleTarget.x - rigidBody.position.x,
                rigidBody.grappleTarget.y - rigidBody.position.y,
                rigidBody.grappleTarget.z - rigidBody.position.z,
            };
            float distance = Vector3Length(toTarget);
            Vector3 wireDir = Vector3Normalize(toTarget);
            float dotVelWire = Vector3DotProduct(rigidBody.velocity, wireDir);

            if (distance <= rigidBody.grappleStopDistance) {
                if (dotVelWire > 0.0f && dotVelWire < 8.0f) {
                    rigidBody.velocity.x -= wireDir.x * dotVelWire;
                    rigidBody.velocity.y -= wireDir.y * dotVelWire;
                    rigidBody.velocity.z -= wireDir.z * dotVelWire;
                }
                StopGrapple(&rigidBody, 1.0f);
                return;
            }

            if (distance >= rigidBody.grappleWireLength && dotVelWire < 0.0f) {
                rigidBody.velocity.x -= wireDir.x * dotVelWire;
                rigidBody.velocity.y -= wireDir.y * dotVelWire;
                rigidBody.velocity.z -= wireDir.z * dotVelWire;
                dotVelWire = 0.0f;
            }

            rigidBody.grappleWireLength =
                std::max(rigidBody.grappleWireLength - 4.0f * delta,
                         rigidBody.grappleStopDistance + 0.5f);

            rigidBody.velocity.x += wireDir.x * 25.0f * delta;
            rigidBody.velocity.y += wireDir.y * 25.0f * delta;
            rigidBody.velocity.z += wireDir.z * 25.0f * delta;

            const float SWING_MAX_SPEED = 30.0f;
            float currentSpeed = Vector3Length(rigidBody.velocity);
            if (currentSpeed > SWING_MAX_SPEED) {
                rigidBody.velocity = Vector3Scale(
                    Vector3Normalize(rigidBody.velocity), SWING_MAX_SPEED);
            }

            Vector3 nextPos = {
                rigidBody.position.x + rigidBody.velocity.x * delta,
                rigidBody.position.y + rigidBody.velocity.y * delta,
                rigidBody.position.z + rigidBody.velocity.z * delta,
            };
            Vector3 newToTarget =
                Vector3Subtract(rigidBody.grappleTarget, nextPos);
            float newDist = Vector3Length(newToTarget);
            if (newDist > 0.001f &&
                Vector3DotProduct(wireDir, Vector3Normalize(newToTarget)) <
                    0.0f) {
                rigidBody.position = nextPos;
                StopGrapple(&rigidBody, 1.0f);
                transform.SetRotation(
                    glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f)));
                transform.SetPosition(aot::RaylibMaths::toGlmVector3(
                    {rigidBody.position.x, rigidBody.position.y,
                     rigidBody.position.z}));
                return;
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
