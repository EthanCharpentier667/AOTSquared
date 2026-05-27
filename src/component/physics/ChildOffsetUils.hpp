/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** ChildOffsetUils
*/

#pragma once

#include "ChildOffset.hpp"
#include "Object.hpp"
#include "Raylib.hpp"
#include "Relationship.hpp"

namespace aot::physics {
    class ChildOffsetUils {
      public:
        static void makeChildFollowParent(Engine::Entity child,
                                          Engine::Entity parent) {
            Relationship::Utils::SetChildOf(child, parent);

            auto *childTransform =
                child.TryGetComponent<Object::Component::Transform>();
            auto *parentTransform =
                parent.TryGetComponent<Object::Component::Transform>();
            if (!childTransform || !parentTransform) {
                Log::Warning(
                    "Cannot make child follow parent: missing Transform "
                    "component");
                return;
            }
            auto &childOffset = child.AddComponent<aot::physics::ChildOffset>();
            glm::quat parentRotInv =
                glm::inverse(parentTransform->GetRotation());
            glm::vec3 childPos = childTransform->GetPosition();
            glm::vec3 parentPos = parentTransform->GetPosition();
            glm::vec3 localPos = parentRotInv * (childPos - parentPos);
            childOffset.positionOffset =
                aot::RaylibMaths::toRayVector3(localPos);
        }
    };

}  // namespace aot::physics