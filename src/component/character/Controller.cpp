#include "Controller.hpp"

namespace aot::character {

    Controller::Controller() : AMonoBehavior() {
    }

    void Controller::Start(Engine::Core &core) {
        ResolveSelf<Controller>(core);
        Log::Info("Controller Start");
        (void)core;
    }

    void Controller::Update(Engine::Core &core) {
        (void)core;

        char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
        char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
        float rot = lookRotation.x;
        crouching = IsKeyDown(KEY_LEFT_CONTROL);
        jumpPressed = IsKeyPressed(KEY_SPACE);

        Vector2 mouseDelta = GetMouseDelta();
        lookRotation.x -= mouseDelta.x * sensitivity.x;
        lookRotation.y += mouseDelta.y * sensitivity.y;

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
        dir = Vector3Lerp(dir, desiredDir, CONTROL * delta);
    }

    void Controller::FixedUpdate(Engine::Core &core) {
        (void)core;
    }

    void Controller::Stop(Engine::Core &core) {
        (void)core;
    }

}  // namespace aot::character
