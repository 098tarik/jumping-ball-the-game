#pragma once

#include "raylib.h"
#include <vector>
#include "../config/Config.h"

/**
 * Platform: Represents a single climbable platform
 * Platforms form the level - player must jump from platform to platform
 * without touching the ground after the first jump
 */
struct Platform
{
    float x;           // Left edge X position (world coordinates)
    float yTop;        // Top surface Y position (where player lands)
    float width;       // Platform width in pixels
    bool counted;      // Has this platform been scored? (prevents double-counting)
};

/**
 * Cloud: Decorative parallax background element
 * Clouds move slower than platforms to create depth illusion
 */
struct Cloud
{
    float x;           // Center X position
    float y;           // Center Y position  
    float w;           // Width
    float h;           // Height
    float speed;       // Horizontal scroll speed (slower than platform scroll)
};

/**
 * Level: Manages the scrolling platform world and background
 * 
 * Responsibilities:
 * - Generate random platforms with increasing height
 * - Scroll platforms and clouds leftward each frame
 * - Recycle off-screen platforms to the right
 * - Collision detection (ball hitting platform sides = death)
 * - Landing resolution (ball landing on platform tops = safe)
 * - Score tracking (award points when ball passes platforms)
 * - Rendering platforms and sky elements with camera offset
 */
class Level
{
public:
    explicit Level(int totalPlatforms);

    /**
     * generate: Create initial platform layout
     * - Generates all platforms starting from right side of screen
     * - Each platform steps up randomly (creating vertical challenge)
     * - Also generates parallax cloud decorations
     */
    void generate(const GameConfig &cfg);
    
    /**
     * scroll: Move all platforms and clouds leftward
     * - Platforms scroll at scrollSpeed
     * - Clouds scroll at their individual slower speeds (parallax effect)
     * - Recycles off-screen platforms to maintain infinite level
     */
    void scroll(float dt, const GameConfig &cfg);
    
    /**
     * awardScore: Check if ball has passed any uncounted platforms
     * Returns number of newly passed platforms (0 if none)
     * Marks platforms as counted to prevent double-scoring
     */
    int awardScore(float ballX, float radius);
    
    /**
     * checkCollision: Detect if ball hits side/bottom of any platform
     * Used for death condition - hitting platform edges = game over
     * Returns true if collision detected
     */
    bool checkCollision(float ballX, float ballY, float radius, const GameConfig &cfg) const;
    
    /**
     * resolveLanding: Handle ball landing on platform tops or ground
     * - Checks if ball should land on any platform top surface
     * - Falls through to ground if no platform underneath
     * - Sets landedOnGround flag if touched ground (not a platform)
     * Returns true if ball landed (stopped falling)
     */
    bool resolveLanding(float ballX, float prevY, float &y, float &vy, float radius, const GameConfig &cfg, bool &landedOnGround);
    
    /**
     * drawPlatforms: Render all platforms with camera offset
     * Camera offset creates vertical scrolling as ball climbs
     */
    void drawPlatforms(const GameConfig &cfg, float cameraOffsetY) const;
    
    /**
     * drawSky: Render background elements (sun, clouds) with camera offset
     * Clouds use parallax scrolling for depth effect
     */
    void drawSky(const GameConfig &cfg, float cameraOffsetY) const;

    // ===== Public Data =====
    int totalPlatforms;              // Total platforms in level (= total points to win)
    std::vector<Platform> platforms; // All platform instances
    std::vector<Cloud> clouds;       // Background cloud decorations
};
