#include "Level.h"
#include <algorithm>

/**
 * Level constructor: Reserve space for platforms
 */
Level::Level(int total) : totalPlatforms(total) {}

/**
 * generate: Create the initial level layout
 * 
 * Platform Generation:
 * - Starts platforms off-screen to the right (at screenWidth + 200)
 * - Each platform is placed with random horizontal gap (minGap to maxGap)
 * - Each platform steps up by random amount (stepUpMin to stepUpMax)
 * - Platform width varies randomly (minPlatformWidth to maxPlatformWidth)
 * - Platforms never go above minPlatformY (stay below screen top)
 * - All platforms start uncounted (counted = false)
 * 
 * Cloud Generation:
 * - Creates cloudCount decorative clouds at random positions
 * - Clouds have random sizes and scroll speeds (parallax effect)
 * - Positioned across screen width and upper half of screen height
 */
void Level::generate(const GameConfig &cfg)
{
    platforms.resize(totalPlatforms);
    clouds.resize(cfg.cloudCount);

    // Platform generation starting point
    float startX = (float)cfg.screenWidth + 200.0f;  // Start off-screen right
    float cursor = startX;                            // Horizontal position tracker
    float yTop = cfg.groundY - 20.0f;                // Start slightly above ground

    // Generate each platform with random spacing and height
    for (int i = 0; i < totalPlatforms; i++)
    {
        // Random horizontal gap from previous platform
        float gap = (float)GetRandomValue((int)cfg.minGap, (int)cfg.maxGap);
        cursor += gap;
        
        // Random platform width
        float width = (float)GetRandomValue((int)cfg.minPlatformWidth, (int)cfg.maxPlatformWidth);
        
        // Random vertical step up (platforms get progressively higher)
        float step = (float)GetRandomValue((int)cfg.stepUpMin, (int)cfg.stepUpMax);
        yTop = std::max(cfg.minPlatformY, yTop - step);  // Don't go above screen top
        
        // Create platform
        platforms[i] = {cursor, yTop, width, false};
    }

    // Generate background clouds for parallax effect
    for (int i = 0; i < cfg.cloudCount; i++)
    {
        float cx = (float)GetRandomValue(0, cfg.screenWidth + 600);
        float cy = (float)GetRandomValue(40, cfg.screenHeight / 2);  // Upper half only
        float cw = (float)GetRandomValue(70, 130);
        float ch = cw * 0.6f;  // Height proportional to width
        float speed = (float)GetRandomValue(15, 35);  // Slower than platform scroll
        clouds[i] = {cx, cy, cw, ch, speed};
    }
}

/**
 * scroll: Move all platforms and clouds leftward to create scrolling world
 * 
 * Platform Scrolling:
 * - All platforms move left at scrollSpeed
 * - Track rightmost platform position for recycling
 * - When platform leaves screen (goes past left edge), recycle it:
 *   - Move it to the right of the rightmost platform
 *   - Give it new random properties (gap, width, height)
 *   - Reset counted flag so it can be scored again
 * 
 * Cloud Parallax:
 * - Clouds move at their individual speeds (slower than platforms)
 * - When cloud leaves left edge, respawn on right with new properties
 * - Creates depth illusion (slower = farther away)
 */
void Level::scroll(float dt, const GameConfig &cfg)
{
    // Find rightmost platform position (for recycling)
    float rightMost = 0.0f;
    for (int i = 0; i < totalPlatforms; i++)
    {
        platforms[i].x -= cfg.scrollSpeed * dt;  // Move left
        if (platforms[i].x > rightMost)
        {
            rightMost = platforms[i].x;
        }
    }

    // Recycle platforms that leave the screen
    for (int i = 0; i < totalPlatforms; i++)
    {
        // Platform is off-screen left (plus margin)
        if (platforms[i].x + platforms[i].width < -60.0f)
        {
            // Generate new random properties
            float gap = (float)GetRandomValue((int)cfg.minGap, (int)cfg.maxGap);
            float width = (float)GetRandomValue((int)cfg.minPlatformWidth, (int)cfg.maxPlatformWidth);
            float step = (float)GetRandomValue((int)cfg.stepUpMin, (int)cfg.stepUpMax);
            float newY = std::max(cfg.minPlatformY, platforms[i].yTop - step);
            
            // Respawn to right of rightmost platform
            platforms[i] = {rightMost + gap, newY, width, false};
            rightMost = platforms[i].x;  // Update rightmost tracker
        }
    }

    // Parallax clouds (slower scrolling for depth)
    for (auto &c : clouds)
    {
        c.x -= c.speed * dt;  // Each cloud has individual speed
        
        // Respawn cloud when it leaves left edge
        if (c.x + c.w < -40.0f)
        {
            c.x = (float)cfg.screenWidth + (float)GetRandomValue(80, 280);
            c.y = (float)GetRandomValue(40, cfg.screenHeight / 2);
            c.w = (float)GetRandomValue(70, 130);
            c.h = c.w * 0.6f;
            c.speed = (float)GetRandomValue(15, 35);
        }
    }
}

/**
 * awardScore: Check which platforms the ball has fully passed
 * 
 * Scoring Logic:
 * - Platform counts as "passed" when its right edge is left of ball's left edge
 * - Each platform can only be counted once (prevents double-scoring)
 * - Returns number of newly passed platforms this frame
 * 
 * Win Condition:
 * - When score reaches totalPlatforms, level is complete
 */
int Level::awardScore(float ballX, float radius)
{
    int gained = 0;
    for (int i = 0; i < totalPlatforms; i++)
    {
        // Check if platform is fully behind the ball and not yet counted
        if (!platforms[i].counted && platforms[i].x + platforms[i].width < ballX - radius)
        {
            platforms[i].counted = true;  // Mark as scored
            gained += 1;
        }
    }
    return gained;
}

/**
 * checkCollision: Detect if ball collides with any platform (sides or bottom)
 * 
 * Collision Detection:
 * - Uses circle-rectangle collision (closest point method)
 * - Finds closest point on platform rectangle to ball center
 * - If distance from ball center to closest point < radius, collision occurred
 * 
 * Game Rule:
 * - Hitting platform sides/bottom = instant death (game over)
 * - Landing on platform top is handled separately by resolveLanding
 */
bool Level::checkCollision(float ballX, float ballY, float radius, const GameConfig &cfg) const
{
    for (int i = 0; i < totalPlatforms; i++)
    {
        // Platform rectangle bounds
        float rx = platforms[i].x;
        float ry = platforms[i].yTop;
        float rw = platforms[i].width;
        float rh = cfg.platformHeight;

        // Find closest point on rectangle to ball center
        float closestX = (ballX < rx) ? rx : (ballX > rx + rw ? rx + rw : ballX);
        float closestY = (ballY < ry) ? ry : (ballY > ry + rh ? ry + rh : ballY);
        
        // Calculate distance from ball center to closest point
        float dx = ballX - closestX;
        float dy = ballY - closestY;
        float distSq = dx * dx + dy * dy;
        
        // Collision if distance < radius
        if (distSq < radius * radius)
        {
            return true;
        }
    }
    return false;
}

/**
 * resolveLanding: Handle ball landing on platform tops or ground
 * 
 * Landing Detection (only when falling - vy >= 0):
 * 1. Check each platform top surface
 * 2. Ball must be:
 *    - Horizontally aligned (ballX between platform left and right edges)
 *    - Just crossing platform top (current Y below, previous Y above)
 * 3. If multiple platforms qualify, land on highest one
 * 4. If no platform, fall to ground
 * 
 * Landing Effects:
 * - Snap ball Y position to land on surface (minus radius)
 * - Set vertical velocity to 0 (stop falling)
 * - Set landedOnGround flag if landed on ground (not platform)
 * 
 * Game Rule:
 * - Landing on platform = safe (refills jumps)
 * - Landing on ground after first jump = death
 * 
 * Returns: true if ball landed (either platform or ground)
 */
bool Level::resolveLanding(float ballX, float prevY, float &y, float &vy, float radius, const GameConfig &cfg, bool &landedOnGround)
{
    float targetY = cfg.groundY;  // Default to ground level
    bool landed = false;
    landedOnGround = false;
    
    // Only check landing when falling (moving downward)
    if (vy >= 0.0f)
    {
        // Check all platform tops
        for (int i = 0; i < totalPlatforms; i++)
        {
            float top = platforms[i].yTop;
            float left = platforms[i].x;
            float right = platforms[i].x + platforms[i].width;
            
            // Check if ball just crossed this platform's top surface
            if (y + radius >= top &&           // Current frame: below or touching top
                prevY + radius <= top &&       // Previous frame: above top
                left <= ballX &&               // Horizontally within platform
                right >= ballX)
            {
                // Land on highest qualifying platform
                if (top < targetY)
                {
                    targetY = top;
                    landed = true;
                }
            }
        }
    }

    // Apply landing or ground collision
    if (landed)
    {
        // Landed on a platform - snap to top surface
        y = targetY - radius;
        vy = 0.0f;
    }
    else if (y > cfg.groundY)
    {
        // Fell past ground level - snap to ground
        y = cfg.groundY;
        vy = 0.0f;
        landed = true;
        landedOnGround = true;  // Flag for death condition
    }

    return landed;
}

/**
 * drawPlatforms: Render all platforms with vertical camera offset
 * 
 * Camera System:
 * - cameraOffsetY shifts all Y coordinates for vertical scrolling
 * - As ball climbs higher, camera follows (offset becomes more negative)
 * - This keeps ball in visible area while showing vertical progress
 */
void Level::drawPlatforms(const GameConfig &cfg, float cameraOffsetY) const
{
    for (int i = 0; i < totalPlatforms; i++)
    {
        float rx = platforms[i].x;
        float ry = platforms[i].yTop - cameraOffsetY;  // Apply camera offset
        DrawRectangle((int)rx, (int)ry, (int)platforms[i].width, (int)cfg.platformHeight, GOLD);
    }
}

/**
 * drawSky: Render background elements (sun and clouds) with camera offset
 * 
 * Background Elements:
 * - Sun: Fixed position in top-left, moves with camera to stay visible
 * - Clouds: Multiple overlapping ellipses create puffy cloud shapes
 * - Camera offset applied so background scrolls with vertical movement
 */
void Level::drawSky(const GameConfig &cfg, float cameraOffsetY) const
{
    // Sun in top-left corner
    DrawCircle(60, (int)(60 - cameraOffsetY), 40, YELLOW);
    
    // Draw each cloud as overlapping ellipses
    for (const auto &c : clouds)
    {
        float cy = c.y - cameraOffsetY;  // Apply camera offset
        
        // Cloud made of 3 overlapping ellipses for puffy appearance
        DrawEllipse((int)c.x, (int)cy, c.w * 0.6f, c.h * 0.6f, WHITE);
        DrawEllipse((int)(c.x + c.w * 0.2f), (int)(cy - c.h * 0.2f), c.w * 0.5f, c.h * 0.5f, WHITE);
        DrawEllipse((int)(c.x - c.w * 0.2f), (int)(cy - c.h * 0.1f), c.w * 0.55f, c.h * 0.55f, WHITE);
    }
}
