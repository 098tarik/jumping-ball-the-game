#pragma once

/**
 * GameConfig: Central configuration for all game parameters
 * This struct holds all tunable values for physics, level generation, and visuals.
 * Adjusting these values changes the game's feel, difficulty, and appearance.
 */
struct GameConfig
{
    // ===== Screen & Display =====
    int screenWidth = 800;          // Initial window width (before fullscreen scaling)
    int screenHeight = 450;         // Initial window height (used as base for physics scaling)
    
    // ===== Ball/Player Properties =====
    float radius = 20.0f;           // Ball radius in pixels
    float groundY = screenHeight - 80.0f;  // Ground level Y position (bottom boundary)
    
    // ===== Physics =====
    float gravity = 1000.0f;        // Downward acceleration (pixels/sec²) - scales with screen height
    float jumpVelocity = -550.0f;   // Initial upward velocity when jumping (negative = up) - scales with screen height
    float maxJumpHold = 0.25f;      // Maximum seconds player can hold jump for extra height
    float jumpHoldAccel = -1200.0f; // Extra upward acceleration while holding jump - scales with screen height
    
    // ===== Scrolling & Speed =====
    float scrollSpeed = 220.0f;     // Horizontal scroll speed (pixels/sec) - platforms move left at this rate
    
    // ===== Platform Generation =====
    int totalPlatforms = 200;       // Total number of platforms to score/complete the level
    float minGap = 260.0f;          // Minimum horizontal gap between platforms (pixels)
    float maxGap = 420.0f;          // Maximum horizontal gap between platforms (pixels)
    float minPlatformWidth = 120.0f;  // Minimum platform width (pixels)
    float maxPlatformWidth = 200.0f;  // Maximum platform width (pixels)
    float platformHeight = 14.0f;   // Platform thickness (pixels)
    float minPlatformY = 20.0f;     // Highest Y position platforms can reach (near screen top)
    float stepUpMin = 15.0f;        // Minimum vertical step up between consecutive platforms
    float stepUpMax = 35.0f;        // Maximum vertical step up between consecutive platforms
    
    // ===== Visual Elements =====
    int cloudCount = 10;            // Number of parallax background clouds
};
