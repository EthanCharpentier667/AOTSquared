#pragma once

#include "core/Core.hpp"
#include "Object.hpp"
#include "Relationship.hpp"
#include "../../component/physics/ChildOffset.hpp"

struct HierarchyNode {
    aot::physics::ChildOffset *childOffset = nullptr;
    Object::Component::Transform *transform = nullptr;
    Relationship::Component::Relationship *relationship = nullptr;
};

/**
 * @brief System that updates child entity transforms to follow their parent.
 *
 * For entities with ChildOffset, Transform, and Relationship components,
 * this system computes the world-space transform based on the parent's
 * transform and the child's local offset.
 */
void ChildFollowParentSystem(Engine::Core &core);
