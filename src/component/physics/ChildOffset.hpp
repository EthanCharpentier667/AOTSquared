#pragma once

#include "Raylib.hpp"

namespace aot::physics {

    /**
     * @brief Component storing the local offset from a parent entity.
     *
     * Used in conjunction with the Relationship plugin to compute world-space
     * transforms for child entities that should follow their parent.
     */
    struct ChildOffset {
        /// Position offset in parent's local space
        Vector3 positionOffset = Vector3(0.0f);

        /// Rotation offset relative to parent's rotation
        Quaternion rotationOffset = Quaternion(1, 0, 0, 0);
    };
}  // namespace aot::physics