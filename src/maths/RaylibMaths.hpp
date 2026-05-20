/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RaylibMaths
*/

#pragma once

#include <glm/gtc/quaternion.hpp>

#include "Engine.hpp"
#include "glm/glm.hpp"

namespace aot {
    class RaylibMaths {
      public:
        static Vector3 toRayVector3(glm::vec3 vec) {
            return {vec.x, vec.y, vec.z};
        }

        static Quaternion toRayQuaternion(glm::quat quat) {
            return {quat.x, quat.y, quat.z, quat.w};
        }

        static glm::vec3 toGlmVector3(Vector3 vec) {
            return {vec.x, vec.y, vec.z};
        }

        static glm::quat toGlmQuaternion(Quaternion quat) {
            return {quat.x, quat.y, quat.z, quat.w};
        }
    };
}  // namespace aot