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

static constexpr float TPS_SCROLL_SPEED = 1.5f;
static constexpr float FPS_THRESHOLD = 0.3f;
static constexpr float TPS_MAX_DISTANCE = 20.0f;

static void UpdateCameraThirdPerson(
    Camera *camera, const Object::Component::Transform &transform,
    aot::camera::RaylibCamera &rigCamera) {
    const Vector3 up = {0.0f, 1.0f, 0.0f};
    const Vector3 targetOffset = {0.0f, 0.0f, -1.0f};

    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouseDelta = GetMouseDelta();
        rigCamera.lookRotation.x -= mouseDelta.x * rigCamera.sensitivity.x;
        rigCamera.lookRotation.y -= mouseDelta.y * rigCamera.sensitivity.y;
        rigCamera.lookRotation.y = Clamp(rigCamera.lookRotation.y, -1.3f, 1.3f);
    }

    Vector3 yaw =
        Vector3RotateByAxisAngle(targetOffset, up, rigCamera.lookRotation.x);

    float pitchAngle =
        Clamp(-rigCamera.lookRotation.y, -0.3f, PI / 2.0f - 0.01f);
    float horizontalDist = rigCamera.tpsDistance * cosf(pitchAngle);
    float verticalDist = rigCamera.tpsDistance * sinf(pitchAngle);

    Vector3 headPos = {
        transform.GetPosition().x,
        transform.GetPosition().y + PLAYER_CENTER_HEIGHT,
        transform.GetPosition().z,
    };

    camera->position = {
        headPos.x - yaw.x * horizontalDist,
        headPos.y + verticalDist,
        headPos.z - yaw.z * horizontalDist,
    };
    camera->target = headPos;
    camera->up = up;
}

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
        transform.GetPosition().y + rigCamera.headLerp,
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

    float scroll = GetMouseWheelMove();
    static bool s_cursorLocked = false;

    cameraView.each([&](auto &raylibCamera, auto &transform) {
        raylibCamera.tpsDistance -= scroll * TPS_SCROLL_SPEED;
        raylibCamera.tpsDistance =
            Clamp(raylibCamera.tpsDistance, 0.0f, TPS_MAX_DISTANCE);

        if (raylibCamera.tpsDistance <= FPS_THRESHOLD)
            raylibCamera.tpsDistance = 0.0f;

        bool isFPS = raylibCamera.tpsDistance == 0.0f;

        if (isFPS && !s_cursorLocked) {
            DisableCursor();
            s_cursorLocked = true;
        } else if (!isFPS && s_cursorLocked) {
            EnableCursor();
            s_cursorLocked = false;
        }

        if (isFPS)
            UpdateCameraFPS(&raylibCamera.camera, transform, raylibCamera);
        else
            UpdateCameraThirdPerson(&raylibCamera.camera, transform,
                                    raylibCamera);
    });
}