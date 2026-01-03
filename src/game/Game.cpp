#include "Game.h"
#include <algorithm>
#include <cmath>

/**
 * Game constructor: Initialize level with configured platform count
 * Player uses default constructor (initialized by reset() later)
 */
Game::Game() : level(config.totalPlatforms) {}

/**
 * run: Main game entry point
 * 
 * Initialization:
 * 1. Enable fullscreen mode before creating window
 * 2. Create window with title (actual size determined by monitor)
 * 3. Scale physics values proportionally to screen height:
 *    - gravity, jumpVelocity, jumpHoldAccel all scale with heightScale
 *    - This keeps gameplay feel consistent across different resolutions
 *    - Base resolution is 450px height, so 1440p ≈ 3.2x multiplier
 * 4. Set target FPS to 60
 * 5. Reset game to starting state
 * 
 * Game Loop:
 * - Runs until user closes window (ESC or window close button)
 * - Each frame: handle input, update state, render
 */
void Game::run()
{
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(config.screenWidth, config.screenHeight, "Side Scroller: Jumping Ball");
    
    // Scale physics to match fullscreen resolution
    // Base height is 450px - if screen is taller, physics need stronger forces
    float heightScale = GetScreenHeight() / 450.0f;
    config.gravity *= heightScale;
    config.jumpVelocity *= heightScale;
    config.jumpHoldAccel *= heightScale;
    
    SetTargetFPS(60);
    reset();

    // Main game loop - runs every frame
    while (!WindowShouldClose())
    {
        handleInput();  // Process keyboard input
        update();       // Update game logic
        draw();         // Render everything
    }

    CloseWindow();
}

/**
 * reset: Initialize/restart game to starting state
 * Called at game start and when player presses space after game over/complete
 */
void Game::reset()
{
    player.reset(config);     // Move ball to starting position, reset jumps
    level.generate(config);   // Create new random platform layout
    score = 0;                // Clear score
    gameOver = false;         // Clear death flag
    levelComplete = false;    // Clear win flag
    cameraOffsetY = 0.0f;     // Reset camera to starting position
}

/**
 * handleInput: Process keyboard input each frame
 * 
 * During Gameplay:
 * - Space: Jump (if jumps available - double jump system)
 * 
 * During Game Over / Level Complete:
 * - Space: Restart game (calls reset())
 */
void Game::handleInput()
{
    // Game over/complete state: wait for restart
    if (gameOver || levelComplete)
    {
        if (IsKeyPressed(KEY_SPACE))
        {
            reset();  // Start new game
        }
        return;  // Don't process jump input
    }

    // Gameplay state: handle jumping
    if (player.canJump() && IsKeyPressed(KEY_SPACE))
    {
        player.startJump(config);
    }
}

/**
 * update: Update all game logic each frame
 * 
 * Physics & Movement:
 * 1. Get delta time (time since last frame)
 * 2. Store previous Y position (needed for landing detection)
 * 3. Update player physics (gravity, jump hold, position, rotation)
 * 4. Scroll level (move platforms/clouds left)
 * 
 * Collision & Landing:
 * 5. Resolve landing on platforms or ground
 *    - Sets player Y to surface level if landing
 *    - Returns whether grounded and whether it was ground (not platform)
 * 6. Update player grounded state (refills jumps if landed)
 * 
 * Camera System:
 * 7. Calculate camera offset to follow ball upward
 *    - Keeps ball at 40% screen height when climbing
 *    - Offset is negative (shifts world down as ball climbs)
 *    - Clamped to 0 minimum (doesn't scroll down past start)
 * 
 * Game Over Conditions:
 * 8. Check if touched ground after first jump (death rule)
 * 9. Check if hit platform sides (death from collision)
 * 
 * Scoring & Win:
 * 10. Award points for platforms passed
 * 11. Check if all platforms passed (win condition)
 */
void Game::update()
{
    float dt = GetFrameTime();  // Time since last frame (typically ~0.016s at 60 FPS)
    
    // Pause updates if game ended
    if (gameOver || levelComplete)
    {
        return;
    }

    // Store previous Y for landing detection
    float prevY = player.y;
    
    // Update physics and movement
    player.update(dt, config);  // Apply gravity, jump, update position and rotation
    level.scroll(dt, config);   // Move platforms and clouds left
    
    // Handle landing on platforms or ground
    bool landedOnGround = false;  // Will be set true if landed on ground (not platform)
    bool groundedNow = level.resolveLanding(
        player.x, prevY, player.y, player.vy, 
        config.radius, config, landedOnGround
    );
    player.setGrounded(groundedNow);  // Update player state, refill jumps if landed

    // Camera follows ball upward
    // desiredScreenY = where we want ball on screen (40% from top)
    // cameraOffsetY = how much to shift world down (negative value)
    float desiredScreenY = config.screenHeight * 0.4f;
    cameraOffsetY = std::min(0.0f, player.y - desiredScreenY);

    // Death condition: touched ground after leaving it at least once
    if (landedOnGround && player.hasJumpedOnce())
    {
        gameOver = true;
    }

    // Award score for passing platforms
    score += level.awardScore(player.x, config.radius);

    // Win condition: passed all platforms
    if (score >= config.totalPlatforms)
    {
        levelComplete = true;
    }

    // Death condition: hit platform side/bottom
    if (level.checkCollision(player.x, player.y, config.radius, config))
    {
        gameOver = true;
    }
}

/**
 * draw: Render all game visuals
 * 
 * Rendering Order (back to front):
 * 1. Clear to teal background color
 * 2. Sky elements (sun, clouds) - with camera offset for vertical scroll
 * 3. Ground rectangle - with camera offset
 * 4. Platforms (gold rectangles) - with camera offset
 * 5. Player ball (red with white dot) - with camera offset
 *    - Ball rotation creates rolling effect
 *    - White dot at 75% radius rotates to show rolling motion
 * 6. UI text (instructions, score) - no camera offset (fixed on screen)
 * 7. Game over / level complete overlays - no camera offset
 * 
 * Camera Offset:
 * - All world elements (sky, ground, platforms, player) use cameraOffsetY
 * - UI elements don't use offset (stay fixed on screen)
 * - Negative offset shifts Y coordinates down (ball climbs, camera follows)
 */
void Game::draw()
{
    BeginDrawing();
    ClearBackground(background);  // Teal background

    // Background elements (sun and clouds) with camera
    level.drawSky(config, cameraOffsetY);

    // Ground rectangle (dark green) with camera
    DrawRectangle(0, (int)(config.groundY + config.radius - cameraOffsetY), config.screenWidth, config.screenHeight, DARKGREEN);
    
    // Platforms (gold) with camera
    level.drawPlatforms(config, cameraOffsetY);
    
    // Player ball (red with rotating white dot)
    float screenY = player.y - cameraOffsetY;  // Apply camera offset to Y
    DrawCircle((int)player.x, (int)screenY, config.radius, RED);
    
    // White dot shows rolling motion (rotates with ball)
    // Positioned at 75% of radius (inside edge but not touching)
    // Rotation matches scroll speed (630 deg/sec = one rotation per circumference at 220 px/sec)
    float spotX = player.x + config.radius * 0.75f * cosf(player.rotation * PI / 180.0f);
    float spotY = screenY + config.radius * 0.75f * sinf(player.rotation * PI / 180.0f);
    DrawCircle((int)spotX, (int)spotY, 4.0f, WHITE);

    // UI text (fixed on screen - no camera offset)
    DrawText("Space to jump", 20, 20, 20, BLACK);
    DrawText(TextFormat("Score: %d / %d", score, config.totalPlatforms), config.screenWidth - 220, 20, 20, BLACK);

    // Game over overlay
    if (gameOver)
    {
        DrawRectangle(0, 0, config.screenWidth, config.screenHeight, Fade(BLACK, 0.45f));
        DrawText("Game Over", config.screenWidth / 2 - 90, config.screenHeight / 2 - 40, 32, WHITE);
        DrawText("Space to restart", config.screenWidth / 2 - 115, config.screenHeight / 2 + 4, 20, WHITE);
    }

    // Level complete overlay
    if (levelComplete)
    {
        DrawRectangle(0, 0, config.screenWidth, config.screenHeight, Fade(DARKGREEN, 0.35f));
        DrawText("Level Complete!", config.screenWidth / 2 - 120, config.screenHeight / 2 - 40, 32, WHITE);
        DrawText("Space to play again", config.screenWidth / 2 - 130, config.screenHeight / 2 + 4, 20, WHITE);
    }

    EndDrawing();
}
