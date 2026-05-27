/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Utils
*/

#pragma once

#include "Raylib.hpp"
#include "Relationship.hpp"
#include "Engine.hpp"

namespace aot::utils {
    /// @brief Try to get a component of type TComponent from the parent entity.
    /// It returns a pointer to the component if it
    ///     exists, or nullptr if it does not exist.
    /// @tparam TComponent The type of the component to get from the parent
    /// entity.
    /// @return The component of type TComponent from the parent entity if it
    /// exists, or nullptr if it does not exist.
    template <typename TComponent>
    decltype(auto) TryGetParentComponent(Engine::Entity entity) {
        auto parentEntityOpt = Relationship::Utils::GetParent(entity);
        if (!parentEntityOpt.has_value()) {
            return static_cast<TComponent *>(nullptr);
        }
        auto parentEntity = parentEntityOpt.value();
        return parentEntity.TryGetComponent<TComponent>();
    }
}  // namespace aot::utils