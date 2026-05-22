/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CameraSystem
*/

#include "CameraSystem.hpp"

#include "../../maths/RaylibMaths.hpp"
#include "Object.hpp"
#include "component/camera/RaylibCamera.hpp"

static void UpdateCameraFPS(Camera *camera,
                            const Object::Component::Transform &transform,
                            aot::camera::RaylibCamera &rigCamera) {
    const Vector3 up = {0.0f, 1.0f, 0.0f};
    const Vector3 targetOffset = {0.0f, 0.0f, -1.0f};

    Vector2 mouseDelta = GetMouseDelta();
    rigCamera.lookRotation.x -= mouseDelta.x * rigCamera.sensitivity.x;
    rigCamera.lookRotation.y += mouseDelta.y * rigCamera.sensitivity.y;

    Vector3 yaw =
        Vector3RotateByAxisAngle(targetOffset, up, rigCamera.lookRotation.x);

    float maxAngleUp = Vector3Angle(up, yaw);
    maxAngleUp -= 0.001f;
    if (-(rigCamera.lookRotation.y) > maxAngleUp) {
        rigCamera.lookRotation.y = -maxAngleUp;
    }

    float maxAngleDown = Vector3Angle(Vector3Negate(up), yaw);
    maxAngleDown *= -1.0f;
    maxAngleDown += 0.001f;
    if (-(rigCamera.lookRotation.y) < maxAngleDown) {
        rigCamera.lookRotation.y = -maxAngleDown;
    }

    Vector3 right = Vector3Normalize(Vector3CrossProduct(yaw, up));
    float pitchAngle = -rigCamera.lookRotation.y - rigCamera.lean.y;
    pitchAngle = Clamp(pitchAngle, -PI / 2 + 0.0001f, PI / 2 - 0.0001f);
    Vector3 pitch = Vector3RotateByAxisAngle(yaw, right, pitchAngle);

    float headSin = sinf(rigCamera.headTimer * PI);
    float headCos = cosf(rigCamera.headTimer * PI);
    const float stepRotation = 0.01f;
    camera->up = Vector3RotateByAxisAngle(
        up, pitch, headSin * stepRotation + rigCamera.lean.x);

    const float bobSide = 0.1f;
    const float bobUp = 0.15f;
    Vector3 bobbing = Vector3Scale(right, headSin * bobSide);
    bobbing.y = fabsf(headCos * bobUp);

    Vector3 basePosition = {
        transform.GetPosition().x,
        transform.GetPosition().y + BOTTOM_HEIGHT + rigCamera.headLerp,
        transform.GetPosition().z,
    };

    camera->position =
        Vector3Add(basePosition, Vector3Scale(bobbing, rigCamera.walkLerp));
    camera->target = Vector3Add(camera->position, pitch);
}

void CameraSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto cameraView =
        registry
            .view<aot::camera::RaylibCamera, Object::Component::Transform>();

    cameraView.each([&](auto &raylibCamera, auto &transform) {
        UpdateCameraFPS(&raylibCamera.camera, transform, raylibCamera);
    });
}