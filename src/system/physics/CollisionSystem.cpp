/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CollisionSystem
*/

#include "CollisionSystem.hpp"

#include <cstdint>
#include <vector>

#include "../../component/physics/ColliderUtils.hpp"
#include "Object.hpp"

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

static bool CanCollide(const aot::physics::BoxCollider &box,
                       const aot::physics::BoxCollider &other) {
    return PassFilter(box.mask, box.tag, other.mask, other.tag);
}

static bool CanCollide(const aot::physics::BoxCollider &box,
                       const aot::physics::SphereCollider &sphere) {
    return PassFilter(box.mask, box.tag, sphere.mask, sphere.tag);
}

static bool CanCollide(const aot::physics::BoxCollider &box,
                       const aot::physics::CapsuleCollider &capsule) {
    return PassFilter(box.mask, box.tag, capsule.mask, capsule.tag);
}

static bool CanCollide(const aot::physics::SphereCollider &sphere,
                       const aot::physics::SphereCollider &other) {
    return PassFilter(sphere.mask, sphere.tag, other.mask, other.tag);
}

static bool CanCollide(const aot::physics::CapsuleCollider &capsule,
                       const aot::physics::BoxCollider &box) {
    return PassFilter(capsule.mask, capsule.tag, box.mask, box.tag);
}

static bool CanCollide(const aot::physics::CapsuleCollider &capsule,
                       const aot::physics::SphereCollider &sphere) {
    return PassFilter(capsule.mask, capsule.tag, sphere.mask, sphere.tag);
}

static bool CanCollide(const aot::physics::CapsuleCollider &capsule,
                       const aot::physics::CapsuleCollider &other) {
    return PassFilter(capsule.mask, capsule.tag, other.mask, other.tag);
}

static void Reset(aot::physics::BoxCollider &collider) {
    collider.isColliding = false;
    collider.gizmoColor = GREEN;
}

static void Reset(aot::physics::SphereCollider &collider) {
    collider.isColliding = false;
    collider.gizmoColor = GREEN;
}

static void Reset(aot::physics::CapsuleCollider &collider) {
    collider.isColliding = false;
    collider.gizmoColor = GREEN;
}

static void MarkCollision(aot::physics::BoxCollider &collider) {
    collider.isColliding = true;
    collider.gizmoColor = RED;
}

static void MarkCollision(aot::physics::SphereCollider &collider) {
    collider.isColliding = true;
    collider.gizmoColor = RED;
}

static void MarkCollision(aot::physics::CapsuleCollider &collider) {
    collider.isColliding = true;
    collider.gizmoColor = RED;
}

static bool CapsuleHitsBox(const aot::physics::CapsuleCollider &capsule,
                           const aot::physics::BoxCollider &box) {
    aot::physics::BoxCollider capsuleBody;
    capsuleBody.position = {capsule.position.x,
                            capsule.position.y + (capsule.height * 0.5f),
                            capsule.position.z};
    capsuleBody.size = {capsule.radius * 2.0f, capsule.GetCylinderHeight(),
                        capsule.radius * 2.0f};

    if (CheckCollisionBoxes(capsuleBody.GetBoundingBox(), box.GetBoundingBox()))
        return true;

    if (CheckCollisionBoxSphere(box.GetBoundingBox(),
                                capsule.GetTopSphereCenter(), capsule.radius))
        return true;

    if (CheckCollisionBoxSphere(box.GetBoundingBox(),
                                capsule.GetBottomSphereCenter(),
                                capsule.radius))
        return true;

    return false;
}

static bool CapsuleHitsSphere(const aot::physics::CapsuleCollider &capsule,
                              const aot::physics::SphereCollider &sphere) {
    aot::physics::BoxCollider capsuleBody;
    capsuleBody.position = {capsule.position.x,
                            capsule.position.y + (capsule.height * 0.5f),
                            capsule.position.z};
    capsuleBody.size = {capsule.radius * 2.0f, capsule.GetCylinderHeight(),
                        capsule.radius * 2.0f};

    if (CheckCollisionBoxSphere(capsuleBody.GetBoundingBox(), sphere.position,
                                sphere.radius))
        return true;

    if (CheckCollisionSpheres(capsule.GetTopSphereCenter(), capsule.radius,
                              sphere.position, sphere.radius))
        return true;

    if (CheckCollisionSpheres(capsule.GetBottomSphereCenter(), capsule.radius,
                              sphere.position, sphere.radius))
        return true;

    return false;
}

static bool CapsuleHitsCapsule(const aot::physics::CapsuleCollider &capsule,
                               const aot::physics::CapsuleCollider &other) {
    aot::physics::BoxCollider capsuleBody;
    capsuleBody.position = {capsule.position.x,
                            capsule.position.y + (capsule.height * 0.5f),
                            capsule.position.z};
    capsuleBody.size = {capsule.radius * 2.0f, capsule.GetCylinderHeight(),
                        capsule.radius * 2.0f};

    aot::physics::BoxCollider otherBody;
    otherBody.position = {other.position.x,
                          other.position.y + (other.height * 0.5f),
                          other.position.z};
    otherBody.size = {other.radius * 2.0f, other.GetCylinderHeight(),
                      other.radius * 2.0f};

    if (CheckCollisionBoxes(capsuleBody.GetBoundingBox(),
                            otherBody.GetBoundingBox()))
        return true;

    if (CheckCollisionSpheres(capsule.GetTopSphereCenter(), capsule.radius,
                              other.GetTopSphereCenter(), other.radius))
        return true;

    if (CheckCollisionSpheres(capsule.GetTopSphereCenter(), capsule.radius,
                              other.GetBottomSphereCenter(), other.radius))
        return true;

    if (CheckCollisionSpheres(capsule.GetBottomSphereCenter(), capsule.radius,
                              other.GetTopSphereCenter(), other.radius))
        return true;

    if (CheckCollisionSpheres(capsule.GetBottomSphereCenter(), capsule.radius,
                              other.GetBottomSphereCenter(), other.radius))
        return true;

    return false;
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
        Reset(box);
        boxes.push_back(BoxProxy{&box});
    });

    spheres.reserve(sphereView.size_hint());
    sphereView.each([&](auto, auto &sphere, auto &transform) {
        sphere.position =
            aot::physics::GetColliderWorldPosition(transform, sphere.offset);
        Reset(sphere);
        spheres.push_back(SphereProxy{&sphere});
    });

    capsules.reserve(capsuleView.size_hint());
    capsuleView.each([&](auto, auto &capsule, auto &transform) {
        capsule.position =
            aot::physics::GetColliderWorldPosition(transform, capsule.offset);
        Reset(capsule);
        capsules.push_back(CapsuleProxy{&capsule});
    });

    for (size_t i = 0; i < boxes.size(); ++i) {
        auto *box = boxes[i].collider;

        for (size_t j = i + 1; j < boxes.size(); ++j) {
            auto *otherBox = boxes[j].collider;
            if (CanCollide(*box, *otherBox) &&
                CheckCollisionBoxes(box->GetBoundingBox(),
                                    otherBox->GetBoundingBox())) {
                MarkCollision(*box);
                MarkCollision(*otherBox);
            }
        }

        for (auto &sphereProxy : spheres) {
            auto *sphere = sphereProxy.collider;
            if (!CanCollide(*box, *sphere))
                continue;

            if (CheckCollisionBoxSphere(box->GetBoundingBox(), sphere->position,
                                        sphere->radius)) {
                MarkCollision(*box);
                MarkCollision(*sphere);
            }
        }

        for (auto &capsuleProxy : capsules) {
            auto *capsule = capsuleProxy.collider;
            if (!CanCollide(*box, *capsule))
                continue;

            if (CapsuleHitsBox(*capsule, *box)) {
                MarkCollision(*box);
                MarkCollision(*capsule);
            }
        }
    }

    for (size_t i = 0; i < spheres.size(); ++i) {
        auto *sphere = spheres[i].collider;

        for (size_t j = i + 1; j < spheres.size(); ++j) {
            auto *otherSphere = spheres[j].collider;
            if (CanCollide(*sphere, *otherSphere) &&
                CheckCollisionSpheres(sphere->position, sphere->radius,
                                      otherSphere->position,
                                      otherSphere->radius)) {
                MarkCollision(*sphere);
                MarkCollision(*otherSphere);
            }
        }
    }

    for (size_t i = 0; i < capsules.size(); ++i) {
        auto *capsule = capsules[i].collider;

        for (auto &sphereProxy : spheres) {
            auto *sphere = sphereProxy.collider;
            if (!CanCollide(*capsule, *sphere))
                continue;

            if (CapsuleHitsSphere(*capsule, *sphere)) {
                MarkCollision(*capsule);
                MarkCollision(*sphere);
            }
        }

        for (size_t j = i + 1; j < capsules.size(); ++j) {
            auto *otherCapsule = capsules[j].collider;

            if (!CanCollide(*capsule, *otherCapsule))
                continue;

            if (CapsuleHitsCapsule(*capsule, *otherCapsule)) {
                MarkCollision(*capsule);
                MarkCollision(*otherCapsule);
            }
        }
    }
}