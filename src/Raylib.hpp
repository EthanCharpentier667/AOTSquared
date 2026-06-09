/*
** EPITECH PROJECT, 2026
** Aot
** File description:
** Raylib
*/

#pragma once

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Movement constants
#define GRAVITY 24.0f
#define MAX_SPEED 20.0f
#define CROUCH_SPEED 5.0f
#define JUMP_FORCE 12.0f
#define MAX_ACCEL 150.0f
// Grounded drag
#define FRICTION 0.86f
// Increasing air drag, increases strafing speed
#define AIR_DRAG 0.98f
// Responsiveness for turning movement direction to looked direction
#define CONTROL 15.0f
#define CROUCH_HEIGHT 0.0f
#define STAND_HEIGHT 1.0f
#define BOTTOM_HEIGHT 0.5f

// Player body dimensions (single source of truth for mesh + collider)
#define PLAYER_HEIGHT 2.5f
#define PLAYER_RADIUS 0.5f
// First person camera height (eyes, near the top of the body)
#define PLAYER_EYE_HEIGHT (PLAYER_HEIGHT * 0.9f)
// Third person camera pivot (middle of the body)
#define PLAYER_CENTER_HEIGHT (PLAYER_HEIGHT * 0.5f)
// How far below the feet the downward ground raycast looks for a surface
#define GROUND_CHECK_DISTANCE 0.15f

#define NORMALIZE_INPUT 0

#include "Engine.hpp"
#include "maths/RaylibMaths.hpp"