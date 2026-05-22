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

    static void UpdateController(character::Controller &controller, float rot) {
        char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
        char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
        controller.crouching = IsKeyDown(KEY_LEFT_CONTROL);
        controller.jumpPressed = IsKeyPressed(KEY_SPACE);

        Vector3 front = (Vector3){sinf(rot), 0.f, cosf(rot)};
        Vector3 right = (Vector3){cosf(-rot), 0.f, sinf(-rot)};

        Vector2 input = (Vector2){(float)sideway, (float)-forward};
#if defined(NORMALIZE_INPUT)
        if (NORMALIZE_INPUT > 0 && (sideway != 0) && (forward != 0))
            input = Vector2Normalize(input);
#endif

        Vector3 desiredDir = (Vector3){
            input.x * right.x + input.y * front.x,
            0.0f,
            input.x * right.z + input.y * front.z,
        };

        float delta = GetFrameTime();
        controller.dir =
            Vector3Lerp(controller.dir, desiredDir, CONTROL * delta);
    }

    static void ApplyControllerForces(character::Controller &controller,
                                      character::Rigidbody &rigidBody,
                                      float rot) {
        (void)rot;

        if (rigidBody.activeGrapple)
            return;

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
        float accel = Clamp(maxSpeed - speed, 0.f, MAX_ACCEL * GetFrameTime());
        hvel.x += controller.dir.x * accel;
        hvel.z += controller.dir.z * accel;

        rigidBody.velocity.x = hvel.x;
        rigidBody.velocity.z = hvel.z;
    }

    void UpdateControllers(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        auto view = registry.view<character::Controller, character::Rigidbody,
                                  Object::Component::Transform>();

        float rot = 0.0f;
        if (cameraView.begin() != cameraView.end()) {
            auto cameraEntity = *cameraView.begin();
            rot = cameraView.get<aot::camera::RaylibCamera>(cameraEntity)
                      .lookRotation.x;
        }

        view.each([&](auto &controller, auto &rigidBody, auto &) {
            UpdateController(controller, rot);
            ApplyControllerForces(controller, rigidBody, rot);
        });
    }

    void RegisterControllerSystems(Engine::Core &core) {
        core.RegisterSystem<Engine::Scheduler::Update>(UpdateControllers);
    }

}  // namespace aot::physics