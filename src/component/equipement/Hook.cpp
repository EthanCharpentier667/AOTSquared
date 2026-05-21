/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Hook
*/

#include "Hook.hpp"

#include <sstream>

#include "Object.hpp"

namespace aot::gear {

    Hook::Hook(std::optional<Engine::Entity> anchor)
        : AMonoBehavior(), _anchor(anchor) {
    }

    void Hook::Start(Engine::Core &core) {
        ResolveSelf<Hook>(core);
        auto &registry = core.GetRegistry();

        _rb = registry.try_get<aot::character::Rigidbody>(self);
        transform = registry.try_get<Object::Component::Transform>(self);
        grappleRaycast = registry.try_get<aot::physics::Raycast>(self);
        grappleLine = registry.try_get<aot::physics::LineRenderer>(self);

        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        if (cameraView.begin() != cameraView.end()) {
            auto cameraEntity = *cameraView.begin();
            _camera = &cameraView.get<aot::camera::RaylibCamera>(cameraEntity);
            cam = _camera->camera;
        } else {
            Log::Error("Camera not found");
        }
    }

    static Vector3 getGuntipPosition(Camera camera,
                                     std::optional<Engine::Entity> entity) {
        if (entity.has_value()) {
            auto transform =
                entity.value().TryGetComponent<Object::Component::Transform>();
            if (transform)
                return aot::RaylibMaths::toRayVector3(transform->GetPosition());
            return getGuntipPosition(camera, std::nullopt);
        } else {
            return camera.position;
        }
        Log::Warning("Failed to get guntip position, returning zero vector");
        return {0.0f, 0.0f, 0.0f};
    }

    void Hook::Update(Engine::Core &core) {
        if (IsKeyPressed(KEY_E) && !grappling) {
            startGrappling(core);
        }
        if (IsKeyReleased(KEY_E) && grappling) {
            stopGrappling();
        }

        if (grapplingCdTimer > 0.0f)
            grapplingCdTimer -= GetFrameTime();

        if (grappling && grappleLine) {
            grappleLine->startPoint = getGuntipPosition(cam, _anchor);
            grappleLine->endPoint = grapplePoint;
        }
    }

    void Hook::FixedUpdate(Engine::Core &core) {
        if (!_rb && !_camera && !grappleRaycast && !grappleLine)
            return;
        auto &registry = core.GetRegistry();
        _rb = registry.try_get<aot::character::Rigidbody>(self);
        grappleRaycast = registry.try_get<aot::physics::Raycast>(self);
        grappleLine = registry.try_get<aot::physics::LineRenderer>(self);

        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        if (cameraView.begin() != cameraView.end()) {
            auto cameraEntity = *cameraView.begin();
            _camera = &cameraView.get<aot::camera::RaylibCamera>(cameraEntity);
            cam = _camera->camera;
        }
    }

    void Hook::startGrappling(Engine::Core &core) {
        if (grapplingCdTimer > 0.0f)
            return;

        grappling = true;

        if (!grappleRaycast) {
            Log::Error("Grapple Raycast not foundy");
            return;
        }
        if (!_camera) {
            Log::Error("Camera not found");
            return;
        }
        if (!transform) {
            Log::Error("Transform not found");
            return;
        }

        Log::Info("Starting grapple");

        grappleRaycast->origin = getGuntipPosition(cam, _anchor);
        grappleRaycast->direction =
            Vector3Normalize(Vector3Subtract(cam.target, cam.position));
        grappleRaycast->maxDistance = maxGrapDistance;
        grappleRaycast->layerMask =
            static_cast<uint32_t>(aot::physics::ColliderTag::Grappleable);
        grappleRaycast->result = {};
        grappleRaycast->active = true;

        Log::Info("Grapple raycast");

        Invoke(grappleTimeDelay, [this, &core]() {
            if (!grappling)
                return;
            if (grappleRaycast->result.hit) {
                grapplePoint = grappleRaycast->result.point;
                if (grappleLine) {
                    grappleLine->enabled = true;
                    grappleLine->startPoint = getGuntipPosition(cam, _anchor);
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
        if (!_rb) {
            Log::Error("Rigidbody not found");
            return;
        }
        Vector3 lowestPoint =
            aot::RaylibMaths::toRayVector3(transform->GetPosition());

        float grapplePointRelativeYPos = grapplePoint.y - lowestPoint.y;
        float highestPointOnArc = grapplePointRelativeYPos + overshootYAxis;
        if (grapplePointRelativeYPos < 0)
            highestPointOnArc = overshootYAxis;
        Log::Info("Grapple point relative Y position: " +
                  std::to_string(grapplePointRelativeYPos));
        _rb->JumpToPosition(grapplePoint, highestPointOnArc);
    }

    void Hook::stopGrappling() {
        grappling = false;
        grapplingCdTimer = grapplingCd;
        if (_rb) {
            _rb->StopGrapple();
        }
        if (grappleRaycast) {
            grappleRaycast->active = false;
        }
        if (grappleLine) {
            grappleLine->enabled = false;
        }
    }
}  // namespace aot::gear
