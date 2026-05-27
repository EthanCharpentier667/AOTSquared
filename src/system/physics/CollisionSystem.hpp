/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** CollisionSystem
*/

#pragma once

#include "../../component/physics/Collider.hpp"
#include "Raylib.hpp"

struct BoxProxy {
    aot::physics::BoxCollider *collider = nullptr;
};

struct SphereProxy {
    aot::physics::SphereCollider *collider = nullptr;
};

struct CapsuleProxy {
    aot::physics::CapsuleCollider *collider = nullptr;
};

void CollisionSystem(Engine::Core &core);