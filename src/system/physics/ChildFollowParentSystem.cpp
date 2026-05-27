#include "ChildFollowParentSystem.hpp"

#include <vector>

#include "../../component/physics/ChildOffset.hpp"
#include "Object.hpp"
#include "Raylib.hpp"
#include "Relationship.hpp"

void ChildFollowParentSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();

    std::vector<HierarchyNode> nodes;

    auto view =
        registry.view<aot::physics::ChildOffset, Object::Component::Transform,
                      Relationship::Component::Relationship>();
    nodes.reserve(view.size_hint());

    view.each([&](auto entity, auto &childOffset, auto &childTransform,
                  auto &relationship) {
        (void)entity;
        nodes.push_back(HierarchyNode{.childOffset = &childOffset,
                                      .transform = &childTransform,
                                      .relationship = &relationship});
    });

    for (size_t iteration = 0; iteration < nodes.size(); ++iteration) {
        for (auto &node : nodes) {
            if (!node.relationship->parent.has_value())
                continue;

            auto parentEntity = node.relationship->parent.value();
            if (!parentEntity
                     .template HasComponents<Object::Component::Transform>()) {
                continue;
            }

            auto &parentTransform =
                parentEntity
                    .template GetComponents<Object::Component::Transform>();

            glm::vec3 worldPosition = parentTransform.GetPosition() +
                                      parentTransform.GetRotation() *
                                          aot::RaylibMaths::toGlmVector3(
                                              node.childOffset->positionOffset);

            glm::quat worldRotation = parentTransform.GetRotation() *
                                      aot::RaylibMaths::toGlmQuaternion(
                                          node.childOffset->rotationOffset);

            node.transform->SetPosition(worldPosition);
            node.transform->SetRotation(worldRotation);
        }
    }
}
