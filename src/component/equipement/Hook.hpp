/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Hook
*/

#pragma once

#include <functional>
#include <vector>

#include "../AMonoBehavior.hpp"
#include "../camera/RaylibCamera.hpp"
#include "../physics/LineRenderer.hpp"
#include "../physics/Raycast.hpp"
#include "Raylib.hpp"

namespace aot::gear {
    class Hook : public AMonoBehavior {
      public:
        using AMonoBehavior::AMonoBehavior;
        ~Hook() override = default;

        void Start(Engine::Core &core) override;
        void Update(Engine::Core &core) override;
        void FixedUpdate(Engine::Core &core) override;

        Camera cam;
        Vector3 guntip;

        float maxGrapDistance = 50.0f;
        float grappleTimeDelay = 0.1f;
        float grapplingCd = 1.0f;

        aot::physics::Raycast *grappleRaycast = nullptr;
        aot::physics::LineRenderer *grappleLine = nullptr;

      private:
        Vector3 grapplePoint;
        float grapplingCdTimer = 0.0f;
        bool grappling = false;

        void startGrappling(Engine::Core &core);
        void executeGrapple(Engine::Core &core);
        void stopGrappling();
    };
}  // namespace aot::gear
