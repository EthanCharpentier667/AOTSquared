#include "RigidbodySystem.hpp"

#include "../component/character/Controller.hpp"
#include "../component/character/Rigidbody.hpp"
#include "Object.hpp"

void RigidbodySystem(Engine::Core &core) {
    auto &registry = core.GetRegistry();

    auto view =
        registry.view<aot::character::Rigidbody, aot::character::Controller,
                      Object::Component::Transform>();

    for (auto entity : view) {
        auto &rigidBody = view.get<aot::character::Rigidbody>(entity);
        auto &controller = view.get<aot::character::Controller>(entity);
        auto &transform = view.get<Object::Component::Transform>(entity);

        float delta = GetFrameTime();

        if (!rigidBody.isGrounded) {
            rigidBody.state = aot::character::MouvementState::Air;
            rigidBody.velocity.y -= GRAVITY * delta;
        }

        if (rigidBody.isGrounded && controller.jumpPressed) {
            rigidBody.state = aot::character::MouvementState::Air;
            rigidBody.velocity.y = JUMP_FORCE;
            rigidBody.isGrounded = false;

            // Sound can be played at this moment
            // SetSoundPitch(fxJump, 1.0f + (GetRandomValue(-100,
            // 100)*0.001)); PlaySound(fxJump);
        }

        // ControllerSystem updates controller.dir, lookRotation, crouching
        // and jumpPressed

        float decel = (rigidBody.isGrounded ? FRICTION : AIR_DRAG);
        Vector3 hvel = (Vector3){rigidBody.velocity.x * decel, 0.0f,
                                 rigidBody.velocity.z * decel};

        float hvelLength = Vector3Length(hvel);  // Magnitude
        if (hvelLength < (MAX_SPEED * 0.01f))
            hvel = (Vector3){0};

        // This is what creates strafing
        float speed = Vector3DotProduct(hvel, controller.dir);

        // Whenever the amount of acceleration to add is clamped by the
        // maximum acceleration constant, a Player can make the speed faster
        // by bringing the direction closer to horizontal velocity angle
        // More info here: https://youtu.be/v3zT3Z5apaM?t=165
        if (controller.crouching) {
            rigidBody.state = aot::character::MouvementState::Crouching;
        } else if (controller.dir.x != 0.0f || controller.dir.z != 0.0f) {
            rigidBody.state = aot::character::MouvementState::Running;
        } else {
            rigidBody.state = aot::character::MouvementState::Walking;
        }
        float maxSpeed = (controller.crouching ? CROUCH_SPEED : MAX_SPEED);
        float accel = Clamp(maxSpeed - speed, 0.f, MAX_ACCEL * delta);
        hvel.x += controller.dir.x * accel;
        hvel.z += controller.dir.z * accel;

        rigidBody.velocity.x = hvel.x;
        rigidBody.velocity.z = hvel.z;

        rigidBody.position.x += rigidBody.velocity.x * delta;
        rigidBody.position.y += rigidBody.velocity.y * delta;
        rigidBody.position.z += rigidBody.velocity.z * delta;

        // Fancy collision system against the floor
        if (rigidBody.position.y <= 0.0f) {
            rigidBody.position.y = 0.0f;
            rigidBody.velocity.y = 0.0f;
            rigidBody.isGrounded = true;  // Enable jumping
        }

        transform.SetRotation(aot::RaylibMaths::toGlmQuaternion(
            {controller.lookRotation.y, controller.lookRotation.x, 0.0f,
             0.0f}));
        transform.SetPosition(aot::RaylibMaths::toGlmVector3(
            {rigidBody.position.x, rigidBody.position.y,
             rigidBody.position.z}));
    }
}