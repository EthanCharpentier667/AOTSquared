#include "ChildFollowParentSystem.hpp"

#include "../component/physics/ChildOffset.hpp"
#include "Object.hpp"
#include "Raylib.hpp"
#include "Relationship.hpp"

void ChildFollowParentSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();

    // Iterate over all entities that have ChildOffset, Transform, and
    // Relationship
    auto view =
        registry.view<aot::physics::ChildOffset, Object::Component::Transform,
                      Relationship::Component::Relationship>();

    for (auto entity : view) {
        auto &childOffset = view.get<aot::physics::ChildOffset>(entity);
        auto &childTransform = view.get<Object::Component::Transform>(entity);
        auto &relationship =
            view.get<Relationship::Component::Relationship>(entity);

        // Skip if no parent
        if (!relationship.parent.has_value()) {
            continue;
        }

        auto parentEntity = relationship.parent.value();

        // Get parent's transform
        if (!parentEntity.HasComponents<Object::Component::Transform>()) {
            continue;
        }

        auto &parentTransform =
            parentEntity.GetComponents<Object::Component::Transform>();

        // Compute child's world position: parentPos + parentRot *
        // childOffset.positionOffset
        glm::vec3 worldPosition =
            parentTransform.GetPosition() +
            parentTransform.GetRotation() *
                aot::RaylibMaths::toGlmVector3(childOffset.positionOffset);

        // Compute child's world rotation: parentRot *
        // childOffset.rotationOffset
        glm::quat worldRotation =
            parentTransform.GetRotation() *
            aot::RaylibMaths::toGlmQuaternion(childOffset.rotationOffset);

        // Update child's transform
        childTransform.SetPosition(worldPosition);
        childTransform.SetRotation(worldRotation);
    }
}
