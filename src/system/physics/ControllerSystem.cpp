/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ControllerSystem
*/

#include "ControllerSystem.hpp"

#include <algorithm>
#include <cmath>

#include "Object.hpp"
#include "component/camera/RaylibCamera.hpp"
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

    void StartGrapple(aot::character::Rigidbody *rigidBody,
                      Vector3 targetPosition, float trajectoryHeight) {
        rigidBody->activeGrapple = true;
        rigidBody->grappleTarget = targetPosition;
        rigidBody->grappleWireLength =
            Vector3Length(Vector3Subtract(targetPosition, rigidBody->position));
        rigidBody->isGrounded = false;
        rigidBody->state = aot::character::MouvementState::Air;

        Vector3 toTarget = Vector3Normalize(
            Vector3Subtract(targetPosition, rigidBody->position));

        float towardVel = Vector3DotProduct(rigidBody->velocity, toTarget);
        float minLaunchSpeed =
            std::clamp(rigidBody->grappleWireLength * 0.25f, 2.0f, 9.0f);
        float launchSpeed = std::max(towardVel, minLaunchSpeed);
        rigidBody->velocity = Vector3Scale(toTarget, launchSpeed);
        rigidBody->velocity.y += std::min(trajectoryHeight * 0.2f, 2.0f);
    }

    void StopGrapple(aot::character::Rigidbody *rigidBody,
                     float momentumMultiplier) {
        rigidBody->activeGrapple = false;
        rigidBody->grappleTarget = rigidBody->position;
        rigidBody->velocity =
            Vector3Scale(rigidBody->velocity, momentumMultiplier);
        rigidBody->state = rigidBody->isGrounded
                               ? aot::character::MouvementState::Idle
                               : aot::character::MouvementState::Air;
    }

    static void UpdateController(character::Controller &controller, float yaw,
                                 float pitch) {
        char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
        char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
        controller.crouching = IsKeyDown(KEY_LEFT_CONTROL);
        controller.jumpPressed = IsKeyPressed(KEY_SPACE);

        Vector3 right = (Vector3){cosf(-yaw), 0.f, sinf(-yaw)};

        Vector2 input = (Vector2){(float)sideway, (float)-forward};
#if defined(NORMALIZE_INPUT)
        if (NORMALIZE_INPUT > 0 && (sideway != 0) && (forward != 0))
            input = Vector2Normalize(input);
#endif

        Vector3 frontH = (Vector3){sinf(yaw), 0.f, cosf(yaw)};
        Vector3 desiredDir = (Vector3){
            input.x * right.x + input.y * frontH.x,
            0.0f,
            input.x * right.z + input.y * frontH.z,
        };

        float cp = cosf(pitch), sp = sinf(pitch);
        Vector3 front3D = {sinf(yaw) * cp, sp, cosf(yaw) * cp};
        Vector3 desiredDir3D = (Vector3){
            input.x * right.x + input.y * front3D.x,
            input.y * front3D.y,
            input.x * right.z + input.y * front3D.z,
        };

        float delta = GetFrameTime();
        controller.dir =
            Vector3Lerp(controller.dir, desiredDir, CONTROL * delta);
        controller.dir3D =
            Vector3Lerp(controller.dir3D, desiredDir3D, CONTROL * delta);
    }

    static void ApplyControllerForces(character::Controller &controller,
                                      character::Rigidbody &rigidBody) {
        if (rigidBody.activeGrapple) {
            if (IsKeyDown(KEY_SPACE)) {
                float spd = Vector3Length(rigidBody.velocity);
                if (spd > 0.5f)
                    rigidBody.velocity =
                        Vector3Scale(Vector3Normalize(rigidBody.velocity),
                                     std::min(spd + 18.0f * GetFrameTime(),
                                              rigidBody.grappleMaxSpeed));
            }

            float fwd = (float)(IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
            float lat = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));

            if (fwd != 0.0f || lat != 0.0f) {
                Vector3 wireDir = Vector3Normalize(Vector3Subtract(
                    rigidBody.grappleTarget, rigidBody.position));
                const Vector3 worldUp = {0.0f, 1.0f, 0.0f};

                float upR = Vector3DotProduct(worldUp, wireDir);
                Vector3 tangUp = {-wireDir.x * upR, 1.0f - wireDir.y * upR,
                                  -wireDir.z * upR};
                float tuLen = Vector3Length(tangUp);
                if (tuLen > 0.001f)
                    tangUp = Vector3Scale(tangUp, 1.0f / tuLen);

                Vector3 tangRight = Vector3CrossProduct(wireDir, worldUp);
                float trLen = Vector3Length(tangRight);
                if (trLen < 0.001f)
                    tangRight = {1.0f, 0.0f, 0.0f};
                else
                    tangRight = Vector3Scale(tangRight, 1.0f / trLen);

                Vector3 arcDir = Vector3Add(Vector3Scale(tangUp, fwd),
                                            Vector3Scale(tangRight, lat));
                float adLen = Vector3Length(arcDir);
                if (adLen > 0.001f) {
                    arcDir = Vector3Scale(arcDir, 1.0f / adLen);
                    float steer = 40.0f * GetFrameTime();
                    rigidBody.velocity.x += arcDir.x * steer;
                    rigidBody.velocity.y += arcDir.y * steer;
                    rigidBody.velocity.z += arcDir.z * steer;
                    float spd = Vector3Length(rigidBody.velocity);
                    if (spd > rigidBody.grappleMaxSpeed)
                        rigidBody.velocity =
                            Vector3Scale(Vector3Normalize(rigidBody.velocity),
                                         rigidBody.grappleMaxSpeed);
                }
            }
            return;
        }

        if (rigidBody.isGrounded) {
            if (controller.jumpPressed) {
                rigidBody.state = aot::character::MouvementState::Air;
                rigidBody.velocity.y = JUMP_FORCE;
                rigidBody.isGrounded = false;
            }
            Vector3 hvel = {rigidBody.velocity.x * FRICTION, 0.0f,
                            rigidBody.velocity.z * FRICTION};
            if (Vector3Length(hvel) < MAX_SPEED * 0.01f)
                hvel = {0};
            float speed = Vector3DotProduct(hvel, controller.dir);

            if (controller.crouching)
                rigidBody.state = aot::character::MouvementState::Crouching;
            else if (controller.dir.x != 0.0f || controller.dir.z != 0.0f)
                rigidBody.state = aot::character::MouvementState::Running;
            else
                rigidBody.state = aot::character::MouvementState::Walking;

            float maxSpeed = controller.crouching ? CROUCH_SPEED : MAX_SPEED;
            float accel =
                Clamp(maxSpeed - speed, 0.f, MAX_ACCEL * GetFrameTime());
            hvel.x += controller.dir.x * accel;
            hvel.z += controller.dir.z * accel;
            rigidBody.velocity.x = hvel.x;
            rigidBody.velocity.z = hvel.z;
            return;
        }

        rigidBody.state = aot::character::MouvementState::Air;

        if (IsKeyDown(KEY_SPACE)) {
            float spd = Vector3Length(rigidBody.velocity);
            if (spd > 0.5f) {
                float boost = 18.0f * GetFrameTime();
                rigidBody.velocity = Vector3Scale(
                    Vector3Normalize(rigidBody.velocity),
                    std::min(spd + boost, rigidBody.grappleMaxSpeed));
            }
        }

        const float AIR_ACCEL = 65.0f * GetFrameTime();
        rigidBody.velocity.x += controller.dir3D.x * AIR_ACCEL;
        rigidBody.velocity.y += controller.dir3D.y * AIR_ACCEL;
        rigidBody.velocity.z += controller.dir3D.z * AIR_ACCEL;

        if (!IsKeyDown(KEY_SPACE)) {
            rigidBody.velocity.x *= AIR_DRAG;
            rigidBody.velocity.z *= AIR_DRAG;
        }

        float spd = Vector3Length(rigidBody.velocity);
        if (spd > rigidBody.grappleMaxSpeed)
            rigidBody.velocity =
                Vector3Scale(Vector3Normalize(rigidBody.velocity),
                             rigidBody.grappleMaxSpeed);
    }

    void UpdateControllers(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        auto view = registry.view<character::Controller, character::Rigidbody,
                                  Object::Component::Transform>();

        float yaw = 0.0f, pitch = 0.0f;
        if (cameraView.begin() != cameraView.end()) {
            auto cameraEntity = *cameraView.begin();
            const auto &cam =
                cameraView.get<aot::camera::RaylibCamera>(cameraEntity);
            yaw = cam.lookRotation.x;
            pitch = -cam.lookRotation.y;
        }

        view.each([&](auto &controller, auto &rigidBody, auto &) {
            UpdateController(controller, yaw, pitch);
            ApplyControllerForces(controller, rigidBody);
        });
    }

    void RegisterControllerSystems(Engine::Core &core) {
        core.RegisterSystem<Engine::Scheduler::Update>(UpdateControllers);
    }

}  // namespace aot::physics