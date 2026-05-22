/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ControllerSystem
*/

#include "ControllerSystem.hpp"

namespace aot::physics {

    void UpdateController(character::Controller &controller) {
        char sideway = (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
        char forward = (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
        float rot = controller.lookRotation.x;
        controller.crouching = IsKeyDown(KEY_LEFT_CONTROL);
        controller.jumpPressed = IsKeyPressed(KEY_SPACE);

        Vector2 mouseDelta = GetMouseDelta();
        controller.lookRotation.x -= mouseDelta.x * controller.sensitivity.x;
        controller.lookRotation.y += mouseDelta.y * controller.sensitivity.y;

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

    void UpdateControllers(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto view = registry.view<character::Controller>();

        view.each([](auto &controller) { UpdateController(controller); });
    }

    void RegisterControllerSystems(Engine::Core &core) {
        core.RegisterSystem<Engine::Scheduler::Update>(UpdateControllers);
    }

}  // namespace aot::physics