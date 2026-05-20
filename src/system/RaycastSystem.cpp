/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** RaycastSystem
*/

#include "RaycastSystem.hpp"

#include "component/physics/Raycast.hpp"

void RaycastSystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();
    auto view = registry.view<aot::physics::Raycast>();

    for (auto entity : view) {
        auto &raycast = view.get<aot::physics::Raycast>(entity);

        if (!raycast.active)
            continue;

        raycast.result = aot::physics::RaycastAgainstColliders(
            raycast.origin, raycast.direction, raycast.maxDistance,
            raycast.layerMask, &core);

        raycast.active = false;
    }
}
