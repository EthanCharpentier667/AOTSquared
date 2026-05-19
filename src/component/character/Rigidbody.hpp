/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Rigidbody
*/

#ifndef RIGIDBODY_HPP_
#define RIGIDBODY_HPP_

#include "Raylib.hpp"

namespace aot::character {
    struct Rigidbody {
        Vector3 position = {0.0f, 0.0f, 0.0f};
        Vector3 velocity = {0.0f, 0.0f, 0.0f};
        bool isGrounded = true;
    };
}  // namespace aot::character
#endif /* !RIGIDBODY_HPP_ */
