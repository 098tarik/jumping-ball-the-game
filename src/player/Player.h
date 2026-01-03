#pragma once

#include "raylib.h"
#include "../config/Config.h"

/**
 * Player: Represents the ball character controlled by the player
 * 
 * This class manages:
 * - Position and velocity (x, y, vy)
 * - Jump mechanics (double jump, jump hold for variable height)
 * - Visual rotation (rolling effect)
 * - Ground/air state tracking
 * 
 * The player scrolls horizontally automatically via the level scroll,
 * and the player controls vertical movement via jumping.
 */
class Player
{
public:
    // ===== Core Methods =====
    
    /**
     * reset: Initialize player to starting position and state
     * Called at game start and when restarting after game over
     */
    void reset(const GameConfig &cfg);
    
    /**
     * startJump: Initiate a jump if jumps remain
     * Consumes one jump from jumpsRemaining (max 2)
     */
    void startJump(const GameConfig &cfg);
    
    /**
     * update: Apply physics (gravity, jump hold) and update rotation
     * Called every frame - handles vertical movement and rolling animation
     */
    void update(float dt, const GameConfig &cfg);
    
    /**
     * canJump: Check if player can jump (has jumps remaining)
     * Returns true if jumpsRemaining > 0
     */
    bool canJump() const;
    
    /**
     * setGrounded: Update grounded state and refill jumps when landing
     * Called by Game after collision detection with platforms/ground
     */
    void setGrounded(bool groundedState);
    
    /**
     * hasJumpedOnce: Check if player has left the ground at least once
     * Used for game over condition - touching ground after first jump = death
     */
    bool hasJumpedOnce() const;

    // ===== Public State (accessed by Game for rendering/collision) =====
    
    float x = 0.0f;              // Horizontal position (fixed at 25% screen width)
    float y = 0.0f;              // Vertical position (world coordinates)
    float vy = 0.0f;             // Vertical velocity (negative = moving up)
    float rotation = 0.0f;       // Visual rotation angle (degrees, for rolling effect)
    int jumpsRemaining = 2;      // Jump charges (0-2, refills on landing)
    bool isJumping = false;      // Currently in jump motion
    bool grounded = true;        // Currently on a platform or ground
    bool hasLeftGround = false;  // Has jumped at least once (used for death condition)
    float jumpHoldTimer = 0.0f;  // Time spent holding jump button (for variable height)
};
