/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Hook
*/

#include "Hook.hpp"

#include <sstream>

namespace aot::gear {

    void Hook::Start(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto view = registry.view<aot::gear::Hook>();

        for (auto entity : view) {
            Log::Info("Hook component found on entity " +
                      std::to_string(entity));
            if (&view.get<aot::gear::Hook>(entity) != this)
                continue;
            if (auto *camera =
                    registry.try_get<aot::camera::RaylibCamera>(entity)) {
                cam = camera->camera;
            }
            grappleRaycast = registry.try_get<aot::physics::Raycast>(entity);
            grappleLine = registry.try_get<aot::physics::LineRenderer>(entity);
            break;
        }
    }

    void Hook::Update(Engine::Core &core) {
        if (IsKeyPressed(KEY_E)) {
            if (!grappling) {
                startGrappling(core);
            } else {
                stopGrappling();
            }
        }

        if (grapplingCdTimer > 0.0f)
            grapplingCdTimer -= GetFrameTime();
    }

    void Hook::FixedUpdate(Engine::Core &core) {
        if (grappleRaycast && grappleLine)
            return;
        auto &registry = core.GetRegistry();
        auto view = registry.view<aot::gear::Hook>();

        for (auto entity : view) {
            if (&view.get<aot::gear::Hook>(entity) != this)
                continue;
            if (auto *camera =
                    registry.try_get<aot::camera::RaylibCamera>(entity)) {
                cam = camera->camera;
            }
            grappleRaycast = registry.try_get<aot::physics::Raycast>(entity);
            grappleLine = registry.try_get<aot::physics::LineRenderer>(entity);
            break;
        }
    }

    void Hook::startGrappling(Engine::Core &core) {
        if (grapplingCdTimer > 0.0f)
            return;

        grappling = true;

        if (!grappleRaycast)
            return;

        auto &registry = core.GetRegistry();
        auto view = registry.view<aot::camera::RaylibCamera>();
        if (view.begin() == view.end())
            return;

        auto entity = *view.begin();
        auto &rayCamera = view.get<aot::camera::RaylibCamera>(entity).camera;

        grappleRaycast->origin = rayCamera.position;
        grappleRaycast->direction = Vector3Normalize(
            Vector3Subtract(rayCamera.target, rayCamera.position));
        grappleRaycast->maxDistance = maxGrapDistance;
        grappleRaycast->layerMask =
            static_cast<uint32_t>(aot::physics::ColliderTag::Grappleable);
        grappleRaycast->active = true;

        Invoke(grappleTimeDelay, [this, &core]() {
            if (grappleRaycast->result.hit) {
                grapplePoint = grappleRaycast->result.point;
                if (grappleLine) {
                    grappleLine->enabled = true;
                    grappleLine->startPoint = grappleRaycast->origin;
                    grappleLine->endPoint = grapplePoint;
                    grappleLine->width = 0.2f;
                    grappleLine->color = BLACK;
                }
                executeGrapple(core);
            } else {
                stopGrappling();
            }
        });
    }

    void Hook::executeGrapple(Engine::Core & /*core*/) {
    }

    void Hook::stopGrappling() {
        grappling = false;
        grapplingCdTimer = grapplingCd;

        if (grappleLine)
            grappleLine->enabled = false;
    }
}  // namespace aot::gear
