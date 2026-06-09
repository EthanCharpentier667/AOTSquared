/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CollisionSystem
*/

#include "CollisionSystem.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "../../component/physics/ColliderUtils.hpp"
#include "Object.hpp"
#include "component/character/Rigidbody.hpp"

static bool PassFilter(uint32_t sourceMask, aot::physics::ColliderTag sourceTag,
                       uint32_t targetMask,
                       aot::physics::ColliderTag targetTag) {
    const uint32_t effectiveSourceMask =
        sourceMask == 0 ? UINT32_MAX : sourceMask;
    const uint32_t effectiveTargetMask =
        targetMask == 0 ? UINT32_MAX : targetMask;

    return (static_cast<uint32_t>(sourceTag) & effectiveTargetMask) != 0 ||
           (static_cast<uint32_t>(targetTag) & effectiveSourceMask) != 0;
}

template <typename ColliderA, typename ColliderB>
static bool CanCollide(const ColliderA &source, const ColliderB &target) {
    return PassFilter(source.mask, source.tag, target.mask, target.tag);
}

static bool IsZeroVector(const Vector3 &vector) {
    return vector.x == 0.0f && vector.y == 0.0f && vector.z == 0.0f;
}

static BoundingBox SphereToBox(const aot::physics::SphereCollider &sphere) {
    Vector3 radiusVector = {sphere.radius, sphere.radius, sphere.radius};
    return {.min = Vector3Subtract(sphere.position, radiusVector),
            .max = Vector3Add(sphere.position, radiusVector)};
}

static Vector3 GetBoxCenter(const BoundingBox &box) {
    return {(box.min.x + box.max.x) * 0.5f, (box.min.y + box.max.y) * 0.5f,
            (box.min.z + box.max.z) * 0.5f};
}

static Vector3 GetMinimumTranslationVector(const BoundingBox &source,
                                           const BoundingBox &target) {
    if (!CheckCollisionBoxes(source, target))
        return {0.0f, 0.0f, 0.0f};

    float overlapX = std::min(source.max.x, target.max.x) -
                     std::max(source.min.x, target.min.x);
    float overlapY = std::min(source.max.y, target.max.y) -
                     std::max(source.min.y, target.min.y);
    float overlapZ = std::min(source.max.z, target.max.z) -
                     std::max(source.min.z, target.min.z);

    Vector3 sourceCenter = GetBoxCenter(source);
    Vector3 targetCenter = GetBoxCenter(target);

    if (overlapX <= overlapY && overlapX <= overlapZ) {
        return {sourceCenter.x < targetCenter.x ? -overlapX : overlapX, 0.0f,
                0.0f};
    }

    if (overlapY <= overlapX && overlapY <= overlapZ) {
        return {0.0f, sourceCenter.y < targetCenter.y ? -overlapY : overlapY,
                0.0f};
    }

    return {0.0f, 0.0f, sourceCenter.z < targetCenter.z ? -overlapZ : overlapZ};
}

static BoundingBox ToBoundingBox(const aot::physics::BoxCollider &collider) {
    return collider.GetBoundingBox();
}

static BoundingBox ToBoundingBox(const aot::physics::SphereCollider &collider) {
    return SphereToBox(collider);
}

static BoundingBox ToBoundingBox(
    const aot::physics::CapsuleCollider &collider) {
    aot::physics::BoxCollider body;
    body.position = {collider.position.x,
                     collider.position.y + (collider.height * 0.5f),
                     collider.position.z};
    body.size = {collider.radius * 2.0f, collider.GetCylinderHeight(),
                 collider.radius * 2.0f};
    return body.GetBoundingBox();
}

template <typename ColliderA, typename ColliderB>
static Vector3 ResolveCollision(const ColliderA &self, const ColliderB &other) {
    return GetMinimumTranslationVector(ToBoundingBox(self),
                                       ToBoundingBox(other));
}

template <typename ColliderT>
static void ApplyCollisionResponse(ColliderT &collider,
                                   aot::character::Rigidbody &rigidBody,
                                   Object::Component::Transform &transform,
                                   const Vector3 &correction) {
    if (IsZeroVector(correction))
        return;

    collider.position = Vector3Add(collider.position, correction);
    rigidBody.position = Vector3Add(rigidBody.position, correction);

    if (correction.y > 0.0f) {
        rigidBody.velocity.y = 0.0f;
    } else if (correction.y < 0.0f) {
        rigidBody.velocity.y = std::max(0.0f, rigidBody.velocity.y);
    }

    if (correction.x != 0.0f)
        rigidBody.velocity.x = 0.0f;
    if (correction.z != 0.0f)
        rigidBody.velocity.z = 0.0f;

    transform.SetPosition(aot::RaylibMaths::toGlmVector3(rigidBody.position));
}

template <typename ColliderT>
static void ResolveColliderAgainstWorld(
    ColliderT &selfCollider, aot::character::Rigidbody &rigidBody,
    Object::Component::Transform &transform, const std::vector<BoxProxy> &boxes,
    const std::vector<SphereProxy> &spheres,
    const std::vector<CapsuleProxy> &capsules) {
    for (int iteration = 0; iteration < 4; ++iteration) {
        bool corrected = false;

        for (const auto &boxProxy : boxes) {
            auto *box = boxProxy.collider;
            if (!CanCollide(selfCollider, *box))
                continue;

            Vector3 correction = ResolveCollision(selfCollider, *box);
            if (IsZeroVector(correction))
                continue;

            ApplyCollisionResponse(selfCollider, rigidBody, transform,
                                   correction);
            corrected = true;
            break;
        }

        if (corrected)
            continue;

        for (const auto &sphereProxy : spheres) {
            auto *sphere = sphereProxy.collider;
            if (!CanCollide(selfCollider, *sphere))
                continue;

            Vector3 correction = ResolveCollision(selfCollider, *sphere);
            if (IsZeroVector(correction))
                continue;

            ApplyCollisionResponse(selfCollider, rigidBody, transform,
                                   correction);
            corrected = true;
            break;
        }

        if (corrected)
            continue;

        for (const auto &capsuleProxy : capsules) {
            auto *capsule = capsuleProxy.collider;
            if constexpr (std::is_same_v<ColliderT,
                                         aot::physics::CapsuleCollider>) {
                if (&selfCollider == capsule)
                    continue;
            }
            if (!CanCollide(selfCollider, *capsule))
                continue;

            Vector3 correction = ResolveCollision(selfCollider, *capsule);
            if (IsZeroVector(correction))
                continue;

            ApplyCollisionResponse(selfCollider, rigidBody, transform,
                                   correction);
            corrected = true;
            break;
        }

        if (!corrected)
            break;
    }
}

void CollisionSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto boxView =
        registry
            .view<aot::physics::BoxCollider, Object::Component::Transform>();
    auto sphereView =
        registry
            .view<aot::physics::SphereCollider, Object::Component::Transform>();
    auto capsuleView = registry.view<aot::physics::CapsuleCollider,
                                     Object::Component::Transform>();

    std::vector<BoxProxy> boxes;
    std::vector<SphereProxy> spheres;
    std::vector<CapsuleProxy> capsules;

    boxes.reserve(boxView.size_hint());
    boxView.each([&](auto, auto &box, auto &transform) {
        box.position =
            aot::physics::GetColliderWorldPosition(transform, box.offset);
        boxes.push_back(BoxProxy{&box});
    });

    spheres.reserve(sphereView.size_hint());
    sphereView.each([&](auto, auto &sphere, auto &transform) {
        sphere.position =
            aot::physics::GetColliderWorldPosition(transform, sphere.offset);
        spheres.push_back(SphereProxy{&sphere});
    });

    capsules.reserve(capsuleView.size_hint());
    capsuleView.each([&](auto, auto &capsule, auto &transform) {
        capsule.position =
            aot::physics::GetColliderWorldPosition(transform, capsule.offset);
        capsules.push_back(CapsuleProxy{&capsule});
    });

    auto boxRigidBodyView =
        registry.view<aot::physics::BoxCollider, Object::Component::Transform,
                      aot::character::Rigidbody>();
    auto sphereRigidBodyView =
        registry
            .view<aot::physics::SphereCollider, Object::Component::Transform,
                  aot::character::Rigidbody>();
    auto capsuleRigidBodyView =
        registry
            .view<aot::physics::CapsuleCollider, Object::Component::Transform,
                  aot::character::Rigidbody>();

    boxRigidBodyView.each(
        [&](auto, auto &collider, auto &transform, auto &rigidBody) {
            ResolveColliderAgainstWorld(collider, rigidBody, transform, boxes,
                                        spheres, capsules);
        });

    sphereRigidBodyView.each(
        [&](auto, auto &collider, auto &transform, auto &rigidBody) {
            ResolveColliderAgainstWorld(collider, rigidBody, transform, boxes,
                                        spheres, capsules);
        });

    capsuleRigidBodyView.each(
        [&](auto, auto &collider, auto &transform, auto &rigidBody) {
            ResolveColliderAgainstWorld(collider, rigidBody, transform, boxes,
                                        spheres, capsules);
        });
}