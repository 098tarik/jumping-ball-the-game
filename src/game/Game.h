#pragma once

#include "raylib.h"
#include "../config/Config.h"
#include "../player/Player.h"
#include "../level/Level.h"

/**
 * Game: Main game controller - orchestrates all gameplay systems
 * 
 * Responsibilities:
 * - Initialize raylib window and configure fullscreen
 * - Run main game loop (input, update, render)
 * - Coordinate Player, Level, and physics interactions
 * - Manage game state (playing, game over, level complete)
 * - Handle camera system for vertical scrolling
 * - Display UI (score, instructions, end screens)
 * 
 * Game Flow:
 * 1. Constructor: Create level with configured platform count
 * 2. run(): Initialize window, enter game loop until closed
 * 3. Game loop: handleInput -> update -> draw (repeat each frame)
 * 4. On game over/complete: wait for restart input
 */
class Game
{
public:
    /**
     * Constructor: Initialize game components
     * - Creates level with totalPlatforms from config
     * - Player and config use default initialization
     */
    Game();
    
    /**
     * run: Main entry point - set up window and run game loop
     * - Configures fullscreen mode
     * - Scales physics to match actual screen height
     * - Runs game loop until window closed
     */
    void run();

private:
    /**
     * reset: Start/restart the game
     * - Resets player to starting position
     * - Generates new random level
     * - Clears score and game state flags
     * - Resets camera offset
     */
    void reset();
    
    /**
     * handleInput: Process player input
     * - Space to jump (double jump system)
     * - Space to restart after game over/complete
     */
    void handleInput();
    
    /**
     * update: Update game state each frame
     * - Apply physics to player
     * - Scroll level (platforms and clouds)
     * - Resolve landing/collision
     * - Update camera to follow ball
     * - Check game over conditions:
     *   - Touching ground after first jump = death
     *   - Hitting platform sides = death
     * - Award score for passed platforms
     * - Check win condition (score >= totalPlatforms)
     */
    void update();
    
    /**
     * draw: Render all visuals
     * - Background color
     * - Sky elements (sun, clouds) with camera offset
     * - Ground rectangle with camera offset
     * - Platforms with camera offset
     * - Player (red ball with white dot) with camera offset
     * - UI text (instructions, score)
     * - Game over / level complete overlays
     */
    void draw();

    // ===== Game State =====
    GameConfig config;           // Configuration values
    Player player;               // The ball character
    Level level;                 // Platform world and background
    Color background{20, 160, 133, 255};  // Teal background color
    int score = 0;               // Platforms passed (0 to totalPlatforms)
    bool gameOver = false;       // Death state (hit ground or platform side)
    bool levelComplete = false;  // Win state (passed all platforms)
    float cameraOffsetY = 0.0f;  // Vertical camera offset (follows player upward)
};
