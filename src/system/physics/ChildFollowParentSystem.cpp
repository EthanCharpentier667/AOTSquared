#include "ChildFollowParentSystem.hpp"

#include "../../component/physics/ChildOffset.hpp"
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

    view.each([&](auto entity, auto &childOffset, auto &childTransform,
                  auto &relationship) {
        // Skip if no parent
        if (!relationship.parent.has_value()) {
            return;
        }

        auto parentEntity = relationship.parent.value();

        // Get parent's transform
        if (!parentEntity
                 .template HasComponents<Object::Component::Transform>()) {
            return;
        }

        auto &parentTransform =
            parentEntity.template GetComponents<Object::Component::Transform>();

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
    });
}
