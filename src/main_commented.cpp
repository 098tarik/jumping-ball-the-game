/**
 * Side Scroller: Jumping Ball
 * 
 * A vertical platformer where you control a red ball climbing golden platforms.
 * 
 * GAMEPLAY:
 * - Use SPACE to jump (double jump system: 2 jumps before landing)
 * - Hold SPACE for variable jump height (up to 0.25 seconds)
 * - Goal: Pass all 200 platforms without touching the ground
 * - Game Over: Touching ground after first jump OR hitting platform sides
 * - Win: Pass all platforms (score 200/200)
 * 
 * FEATURES:
 * - Fullscreen mode with resolution-scaled physics
 * - Vertical camera following ball upward
 * - Parallax cloud backgrounds
 * - Rolling ball animation (rotation matches scroll speed)
 * - Infinite scrolling (platforms recycle when off-screen)
 * - Random platform heights and gaps
 * 
 * ARCHITECTURE:
 * - Config: All tunable parameters (physics, level generation, visuals)
 * - Player: Ball character (position, velocity, jumps, rotation)
 * - Level: Platform world (generation, scrolling, collision, rendering)
 * - Game: Main controller (game loop, input, state management, camera)
 * - main: Entry point (creates Game and calls run())
 * 
 * CONTROLS:
 * - SPACE: Jump / Restart
 * - ESC: Quit
 */

#include "game/Game.h"

int main()
{
    Game game;   // Create game instance
    game.run();  // Start game loop (initializes window, runs until closed)
    return 0;
}
