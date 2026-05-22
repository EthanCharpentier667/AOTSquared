/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** HookSystem
*/

#include "HookSystem.hpp"

#include "Object.hpp"
#include "component/camera/RaylibCamera.hpp"
#include "component/character/Rigidbody.hpp"
#include "component/equipement/Hook.hpp"
#include "component/physics/LineRenderer.hpp"
#include "component/physics/Raycast.hpp"
#include "system/physics/ControllerSystem.hpp"

namespace aot::physics {

    static Vector3 GetGuntipPosition(Camera camera,
                                     std::optional<Engine::Entity> entity) {
        if (entity.has_value()) {
            auto transform =
                entity.value().TryGetComponent<Object::Component::Transform>();
            if (transform)
                return aot::RaylibMaths::toRayVector3(transform->GetPosition());
            return camera.position;
        }
        return camera.position;
    }

    static void StopGrappling(aot::gear::Hook &hook,
                              aot::character::Rigidbody &rigidBody,
                              aot::physics::Raycast *grappleRaycast,
                              aot::physics::LineRenderer *grappleLine) {
        hook.grappling = false;
        hook.grappleDelayTimer = 0.0f;
        hook.grapplingCdTimer = hook.grapplingCd;
        aot::physics::StopGrapple(rigidBody);

        if (grappleRaycast) {
            grappleRaycast->active = false;
        }
        if (grappleLine) {
            grappleLine->enabled = false;
        }
    }

    static void ExecuteGrapple(aot::gear::Hook &hook,
                               aot::character::Rigidbody &rigidBody,
                               Object::Component::Transform &transform,
                               const Camera &camera) {
        Vector3 lowestPoint =
            aot::RaylibMaths::toRayVector3(transform.GetPosition());

        float grapplePointRelativeYPos = hook.grapplePoint.y - lowestPoint.y;
        float highestPointOnArc =
            grapplePointRelativeYPos + hook.overshootYAxis;
        if (grapplePointRelativeYPos < 0)
            highestPointOnArc = hook.overshootYAxis;

        aot::physics::StartGrapple(rigidBody, hook.grapplePoint,
                                   highestPointOnArc);
        (void)camera;
    }

    static void StartGrappling(aot::gear::Hook &hook,
                               aot::physics::Raycast &grappleRaycast,
                               aot::physics::LineRenderer *grappleLine,
                               const Camera &camera) {
        if (hook.grapplingCdTimer > 0.0f)
            return;

        hook.grappling = true;
        hook.grappleDelayTimer = hook.grappleTimeDelay;

        grappleRaycast.origin = GetGuntipPosition(camera, hook.anchor);
        grappleRaycast.direction =
            Vector3Normalize(Vector3Subtract(camera.target, camera.position));
        grappleRaycast.maxDistance = hook.maxGrapDistance;
        grappleRaycast.layerMask =
            static_cast<uint32_t>(aot::physics::ColliderTag::Grappleable);
        grappleRaycast.result = {};
        grappleRaycast.active = true;

        if (grappleLine) {
            grappleLine->enabled = false;
        }
    }

    void UpdateHooks(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        if (cameraView.begin() == cameraView.end())
            return;

        auto cameraEntity = *cameraView.begin();
        const Camera camera =
            cameraView.get<aot::camera::RaylibCamera>(cameraEntity).camera;

        auto view =
            registry.view<aot::gear::Hook, aot::character::Rigidbody,
                          aot::physics::Raycast, aot::physics::LineRenderer,
                          Object::Component::Transform>();

        float delta = GetFrameTime();
        view.each([&](auto &hook, auto &rigidBody, auto &grappleRaycast,
                      auto &grappleLine, auto &transform) {
            if (IsKeyPressed(KEY_E) && !hook.grappling) {
                StartGrappling(hook, grappleRaycast, &grappleLine, camera);
            }
            if (IsKeyReleased(KEY_E) && hook.grappling) {
                StopGrappling(hook, rigidBody, &grappleRaycast, &grappleLine);
            }

            if (hook.grapplingCdTimer > 0.0f)
                hook.grapplingCdTimer -= delta;

            if (hook.grappleDelayTimer > 0.0f) {
                hook.grappleDelayTimer -= delta;
                if (hook.grappleDelayTimer <= 0.0f && hook.grappling) {
                    if (grappleRaycast.result.hit) {
                        hook.grapplePoint = grappleRaycast.result.point;
                        grappleLine.enabled = true;
                        grappleLine.startPoint =
                            GetGuntipPosition(camera, hook.anchor);
                        grappleLine.endPoint = hook.grapplePoint;
                        grappleLine.width = 0.2f;
                        grappleLine.color = BLACK;
                        ExecuteGrapple(hook, rigidBody, transform, camera);
                    } else {
                        StopGrappling(hook, rigidBody, &grappleRaycast,
                                      &grappleLine);
                    }
                }
            }

            if (hook.grappling) {
                grappleLine.startPoint = GetGuntipPosition(camera, hook.anchor);
                grappleLine.endPoint = hook.grapplePoint;
            }
        });
    }

    void RegisterHookSystems(Engine::Core &core) {
        core.RegisterSystem<Engine::Scheduler::Update>(UpdateHooks);
    }

}  // namespace aot::physics