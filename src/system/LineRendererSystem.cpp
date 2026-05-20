/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** LineRendererSystem
*/

#include "LineRendererSystem.hpp"

#include "../component/camera/RaylibCamera.hpp"

void LineRendererSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto cameraView = registry.view<aot::camera::RaylibCamera>();
    auto lineView = registry.view<aot::physics::LineRenderer>();

    for (auto cameraEntity : cameraView) {
        auto &camera =
            cameraView.get<aot::camera::RaylibCamera>(cameraEntity).camera;

        BeginMode3D(camera);
        for (auto lineEntity : lineView) {
            auto &line = lineView.get<aot::physics::LineRenderer>(lineEntity);

            if (!line.enabled)
                continue;

            Vector3 start = line.startPoint;
            Vector3 end = line.endPoint;
            Vector3 delta = Vector3Subtract(end, start);
            float length = Vector3Length(delta);

            if (length <= 0.0001f)
                continue;

            Vector3 forward = Vector3Scale(delta, 1.0f / length);
            start = Vector3Add(start, Vector3Scale(forward, 0.05f));

            aot::physics::DrawThickLine3D(start, end, line.width * 0.5f,
                                          line.color);
        }
        EndMode3D();
    }
}