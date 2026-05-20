/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Hook
*/

#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "../AMonoBehavior.hpp"
#include "../camera/RaylibCamera.hpp"
#include "../character/Rigidbody.hpp"
#include "../physics/LineRenderer.hpp"
#include "../physics/Raycast.hpp"
#include "Raylib.hpp"

namespace Object::Component {
    class Transform;
}

namespace aot::gear {
    class Hook : public AMonoBehavior {
      public:
        Hook(std::optional<Engine::Entity> anchor = std::nullopt);
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
        aot::camera::RaylibCamera *_camera = nullptr;
        aot::character::Rigidbody *_rb = nullptr;
        Object::Component::Transform *transform = nullptr;

      private:
        std::optional<Engine::Entity> _anchor;
        Vector3 grapplePoint;
        float grapplingCdTimer = 0.0f;
        bool grappling = false;
        float overshootYAxis = 5.0f;

        void startGrappling(Engine::Core &core);
        void executeGrapple(Engine::Core &core);
        void stopGrappling();
    };
}  // namespace aot::gear
