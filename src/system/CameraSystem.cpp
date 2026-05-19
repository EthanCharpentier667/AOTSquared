/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CameraSystem
*/

#include "CameraSystem.hpp"

#include "component/camera/RaylibCamera.hpp"
#include "component/character/Controller.hpp"
#include "component/character/Rigidbody.hpp"

static void UpdateCameraFPS(Camera *camera,
                            const aot::character::Rigidbody &rigidBody,
                            aot::character::Controller &controller) {
    const Vector3 up = (Vector3){0.0f, 1.0f, 0.0f};
    const Vector3 targetOffset = (Vector3){0.0f, 0.0f, -1.0f};

    // Left and right
    Vector3 yaw =
        Vector3RotateByAxisAngle(targetOffset, up, controller.lookRotation.x);

    // Clamp view up
    float maxAngleUp = Vector3Angle(up, yaw);
    maxAngleUp -= 0.001f;  // Avoid numerical errors
    if (-(controller.lookRotation.y) > maxAngleUp) {
        controller.lookRotation.y = -maxAngleUp;
    }

    // Clamp view down
    float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
    maxAngleDown *= -1.0f;   // Downwards angle is negative
    maxAngleDown += 0.001f;  // Avoid numerical errors
    if (-(controller.lookRotation.y) < maxAngleDown) {
        controller.lookRotation.y = -maxAngleDown;
    }

    // Up and down
    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));

    // Rotate view vector around right axis
    float pitchAngle = -controller.lookRotation.y - controller.lean.y;
    pitchAngle = Clamp(pitchAngle, -PI / 2 + 0.0001f,
                       PI / 2 - 0.0001f);  // Clamp angle so it doesn't go past
                                           // straight up or straight down
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, pitchAngle);

    // Head animation
    // Rotate up direction around forward axis
    float headSin = sinf(controller.headTimer * PI);
    float headCos = cosf(controller.headTimer * PI);
    const float stepRotation = 0.01f;
    camera->up = Vector3RotateByAxisAngle(
        up, pitch, headSin * stepRotation + controller.lean.x);

    // Camera BOB
    const float bobSide = 0.1f;
    const float bobUp = 0.15f;
    Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
    bobbing.y = fabsf(headCos * bobUp);

    Vector3 basePosition = {
        rigidBody.position.x,
        rigidBody.position.y + BOTTOM_HEIGHT + controller.headLerp,
        rigidBody.position.z,
    };

    camera->position =
        Vector3Add(basePosition, Vector3Scale(bobbing, controller.walkLerp));
    camera->target = Vector3Add(camera->position, pitch);
}

void CameraSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto playerView =
        registry.view<aot::character::Rigidbody, aot::character::Controller>();

    for (auto playerEntity : playerView) {
        const auto &rigidBody =
            playerView.get<aot::character::Rigidbody>(playerEntity);
        auto &controller =
            playerView.get<aot::character::Controller>(playerEntity);

        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        for (auto cameraEntity : cameraView) {
            auto &camera =
                cameraView.get<aot::camera::RaylibCamera>(cameraEntity).camera;
            camera.position = (Vector3){
                rigidBody.position.x,
                rigidBody.position.y + BOTTOM_HEIGHT + controller.headLerp,
                rigidBody.position.z};
            UpdateCameraFPS(&camera, rigidBody, controller);
        }
    }
}