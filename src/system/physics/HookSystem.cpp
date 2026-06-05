/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** HookSystem
*/

#include "HookSystem.hpp"

#include <array>

#include "Object.hpp"
#include "Relationship.hpp"
#include "component/camera/RaylibCamera.hpp"
#include "component/character/Rigidbody.hpp"
#include "component/equipement/Hook.hpp"
#include "component/physics/LineRenderer.hpp"
#include "system/physics/ControllerSystem.hpp"

namespace aot::physics {

    static Vector3 GetGuntipPosition(Camera camera, Engine::Entity &entity) {
        auto transform = entity.TryGetComponent<Object::Component::Transform>();
        if (transform)
            return aot::RaylibMaths::toRayVector3(transform->GetPosition());
        Log::Warning("Hook anchor entity does not have a Transform component");
        return camera.position;
    }

    static void StopGrappling(aot::gear::Hook &hook,
                              aot::character::Rigidbody *rigidBody,
                              aot::physics::LineRenderer *grappleLine,
                              float momentumMultiplier) {
        hook.grappling = false;
        hook.grappleDelayTimer = 0.0f;
        hook.grapplingCdTimer = hook.grapplingCd;
        aot::physics::StopGrapple(rigidBody, momentumMultiplier);
        if (grappleLine) {
            grappleLine->enabled = false;
        }
    }

    static aot::physics::RaycastHit AimAssistRaycast(
        Vector3 startPoint, Vector3 direction, float maxDistance,
        uint32_t layerMask, const Camera &camera, Engine::Core &core) {
        aot::physics::RaycastHit bestHit = aot::physics::Raycast(
            startPoint, direction, maxDistance, layerMask, &core);

        Vector3 forward = Vector3Normalize(direction);
        Vector3 up = Vector3Normalize(camera.up);
        if (Vector3LengthSqr(up) <= 0.000001f)
            up = {0.0f, 1.0f, 0.0f};

        Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, up));
        if (Vector3LengthSqr(right) <= 0.000001f)
            right = {1.0f, 0.0f, 0.0f};

        up = Vector3Normalize(Vector3CrossProduct(right, forward));

        constexpr float assistCone = 0.045f;
        constexpr std::array<Vector2, 8> sampleOffsets = {{
            {1.0f, 0.0f},
            {-1.0f, 0.0f},
            {0.0f, 1.0f},
            {0.0f, -1.0f},
            {0.7071f, 0.7071f},
            {-0.7071f, 0.7071f},
            {0.7071f, -0.7071f},
            {-0.7071f, -0.7071f},
        }};

        for (const Vector2 &offset : sampleOffsets) {
            Vector3 candidateDirection = Vector3Add(
                forward, Vector3Add(Vector3Scale(right, offset.x * assistCone),
                                    Vector3Scale(up, offset.y * assistCone)));
            candidateDirection = Vector3Normalize(candidateDirection);

            auto hit = aot::physics::Raycast(startPoint, candidateDirection,
                                             maxDistance, layerMask, &core);
            if (hit.hit && (!bestHit.hit || hit.distance < bestHit.distance))
                bestHit = hit;
        }

        return bestHit;
    }

    static void ExecuteGrapple(aot::gear::Hook &hook,
                               aot::character::Rigidbody *rigidBody,
                               Object::Component::Transform *transform) {
        Vector3 lowestPoint =
            aot::RaylibMaths::toRayVector3(transform->GetPosition());

        float grapplePointRelativeYPos = hook.grapplePoint.y - lowestPoint.y;
        float highestPointOnArc =
            grapplePointRelativeYPos + hook.overshootYAxis;
        if (grapplePointRelativeYPos < 0)
            highestPointOnArc = hook.overshootYAxis;

        aot::physics::StartGrapple(rigidBody, hook.grapplePoint,
                                   highestPointOnArc);
    }

    static void StartGrappling(Engine::Core &core, aot::gear::Hook &hook,
                               aot::physics::LineRenderer *grappleLine,
                               const Camera &camera, Engine::Entity &entity,
                               bool isTPS) {
        if (hook.grapplingCdTimer > 0.0f)
            return;

        hook.grappling = true;
        hook.grappleDelayTimer = hook.grappleTimeDelay;

        Vector3 startPoint = GetGuntipPosition(camera, entity);
        Vector3 direction;
        if (isTPS) {
            Ray mouseRay = GetMouseRay(GetMousePosition(), camera);
            auto aimHit = aot::physics::Raycast(
                mouseRay.position, mouseRay.direction, hook.maxGrapDistance,
                static_cast<uint32_t>(aot::physics::ColliderTag::Grappleable),
                &core);
            Vector3 aimTarget =
                aimHit.hit ? aimHit.point
                           : Vector3Add(mouseRay.position,
                                        Vector3Scale(mouseRay.direction,
                                                     hook.maxGrapDistance));
            direction =
                Vector3Normalize(Vector3Subtract(aimTarget, startPoint));
        } else {
            direction = Vector3Normalize(
                Vector3Subtract(camera.target, camera.position));
        }

        hook.grappleHit = AimAssistRaycast(
            startPoint, direction, hook.maxGrapDistance,
            static_cast<uint32_t>(aot::physics::ColliderTag::Grappleable),
            camera, core);

        if (grappleLine) {
            grappleLine->startPoint = startPoint;
            grappleLine->endPoint = hook.grappleHit.point;
            grappleLine->enabled = false;
        }
    }

    void UpdateHooks(Engine::Core &core) {
        auto &registry = core.GetRegistry();
        auto cameraView = registry.view<aot::camera::RaylibCamera>();
        if (cameraView.begin() == cameraView.end())
            return;

        auto cameraEntity = *cameraView.begin();
        const auto &raylibCam =
            cameraView.get<aot::camera::RaylibCamera>(cameraEntity);
        const Camera camera = raylibCam.camera;
        const bool isTPS = raylibCam.tpsDistance > 0.0f;

        auto view = registry.view<aot::gear::Hook>();

        for (auto [entityId, hook] : view.each()) {
            float delta = GetFrameTime();
            Engine::Entity entity{core, entityId};
            aot::physics::LineRenderer *grappleLine =
                entity.template TryGetComponent<aot::physics::LineRenderer>();
            if (!grappleLine) {
                Log::Warning(
                    "No LineRenderer component found on the hook entity");
                return;
            }
            auto *rigidBody = Relationship::Utils::TryGetParentComponent<
                aot::character::Rigidbody>(entity);
            if (!rigidBody) {
                Log::Warning(
                    "No Rigidbody component found on the hook entity's parent");
                return;
            }

            auto *parentTransform = Relationship::Utils::TryGetParentComponent<
                Object::Component::Transform>(entity);
            if (!parentTransform) {
                Log::Warning(
                    "No Transform component found on the hook entity's parent");
                return;
            }
            if (hook.grappling && !rigidBody->activeGrapple &&
                hook.grappleDelayTimer <= 0.0f) {
                hook.grappling = false;
                hook.grapplingCdTimer = hook.grapplingCd;
                grappleLine->enabled = false;
            }

            if (IsKeyPressed(hook.key) && !hook.grappling) {
                Log::Info("Starting grapple");
                StartGrappling(core, hook, grappleLine, camera, entity, isTPS);
            }
            if (IsKeyReleased(hook.key) && hook.grappling) {
                StopGrappling(hook, rigidBody, grappleLine, 1.12f);
            }

            if (hook.grapplingCdTimer > 0.0f)
                hook.grapplingCdTimer -= delta;

            if (hook.grappleDelayTimer > 0.0f) {
                hook.grappleDelayTimer -= delta;
                if (hook.grappleDelayTimer <= 0.0f && hook.grappling) {
                    if (hook.grappleHit.hit) {
                        hook.grapplePoint = hook.grappleHit.point;
                        grappleLine->enabled = true;
                        grappleLine->startPoint =
                            GetGuntipPosition(camera, entity);
                        grappleLine->endPoint = hook.grapplePoint;
                        grappleLine->width = 0.2f;
                        grappleLine->color = BLACK;
                        ExecuteGrapple(hook, rigidBody, parentTransform);
                    } else {
                        StopGrappling(hook, rigidBody, grappleLine, 1.0f);
                    }
                }
            }

            if (hook.grappling) {
                grappleLine->startPoint = GetGuntipPosition(camera, entity);
                grappleLine->endPoint = hook.grapplePoint;
            }
        }
    }

    void RegisterHookSystems(Engine::Core &core) {
        core.RegisterSystem<Engine::Scheduler::Update>(UpdateHooks);
    }

}  // namespace aot::physics