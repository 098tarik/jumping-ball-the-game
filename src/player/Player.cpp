#include "Player.h"

/**
 * reset: Initialize player to starting state
 * - Position: 25% from left edge, at ground level
 * - Velocity: Zero (not moving)
 * - Jumps: Full (2 available)
 * - State: Grounded, not jumping, hasn't left ground yet
 */
void Player::reset(const GameConfig &cfg)
{
    x = cfg.screenWidth * 0.25f;  // Fixed horizontal position (doesn't change during gameplay)
    y = cfg.groundY;               // Start at ground level
    vy = 0.0f;                     // No initial velocity
    rotation = 0.0f;               // No rotation offset
    jumpsRemaining = 2;            // Start with double jump available
    isJumping = false;             // Not currently jumping
    grounded = true;               // Starting on the ground
    hasLeftGround = false;         // Haven't jumped yet (touching ground is safe)
    jumpHoldTimer = 0.0f;          // No jump hold time
}

/**
 * startJump: Execute a jump
 * - Checks if jumps remain (double jump system: 2 jumps before needing to land)
 * - Applies initial upward velocity (negative Y = up)
 * - Consumes one jump charge
 * - Marks player as having left ground (enables death-on-ground-touch rule)
 */
void Player::startJump(const GameConfig &cfg)
{
    // Can't jump if no jumps remaining
    if (jumpsRemaining <= 0)
    {
        return;
    }
    
    // Apply initial jump velocity (negative = upward)
    vy = cfg.jumpVelocity;
    
    // Update state
    isJumping = true;              // Now in air
    grounded = false;              // No longer on surface
    hasLeftGround = true;          // Mark that we've jumped (ground becomes lethal)
    jumpsRemaining--;              // Consume one jump charge
    jumpHoldTimer = 0.0f;          // Reset hold timer for variable height
}

/**
 * update: Apply physics and update visual rotation each frame
 * 
 * Physics:
 * 1. Apply gravity (pulls ball downward)
 * 2. If holding jump button during jump, apply extra upward acceleration
 *    (variable jump height - hold longer = jump higher)
 * 3. Update vertical position based on velocity
 * 
 * Visuals:
 * 4. Rotate ball to match scroll speed (630 deg/sec matches 220 px/sec scroll)
 *    This creates realistic rolling motion as the world scrolls left
 */
void Player::update(float dt, const GameConfig &cfg)
{
    // Apply gravity (constant downward acceleration)
    vy += cfg.gravity * dt;
    
    // Variable jump height: holding space adds extra upward acceleration
    // Limited by maxJumpHold (0.25 sec) to prevent infinite height
    if (IsKeyDown(KEY_SPACE) && isJumping && jumpHoldTimer < cfg.maxJumpHold)
    {
        vy += cfg.jumpHoldAccel * dt;  // Additional upward push
        jumpHoldTimer += dt;            // Track how long we've held
    }
    
    // Update vertical position based on velocity
    y += vy * dt;
    
    // Visual rotation for rolling effect
    // 630 deg/sec matches scroll speed: one full rotation per ball circumference
    // Formula: 360 * scrollSpeed / (2 * PI * radius) ≈ 630
    rotation += 630.0f * dt;
    if (rotation >= 360.0f) rotation -= 360.0f;  // Keep angle in 0-360 range
}

/**
 * canJump: Query if player can currently jump
 * Returns true if player has at least one jump charge remaining
 */
bool Player::canJump() const
{
    return jumpsRemaining > 0;
}

/**
 * setGrounded: Update grounded state after collision detection
 * Called by Game after checking platform/ground collisions
 * 
 * When landing (groundedState = true):
 * - Refills jump charges to 2 (resets double jump)
 * - Clears isJumping flag
 */
void Player::setGrounded(bool groundedState)
{
    grounded = groundedState;
    
    // Landing on a platform/ground refills jumps
    if (groundedState)
    {
        isJumping = false;      // No longer in jump motion
        jumpsRemaining = 2;     // Restore double jump
    }
}

/**
 * hasJumpedOnce: Check if player has ever left the ground
 * Used for game rule: touching ground after first jump = death
 * Returns true if player has jumped at least once since game start/reset
 */
bool Player::hasJumpedOnce() const
{
    return hasLeftGround;
}
