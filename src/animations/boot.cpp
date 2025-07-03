/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "boot.h"

#include <ncurses.h>

#include <cmath>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../util/common.h"

constexpr int FRAME_COUNT = 200;  // Much shorter for a single wave

constexpr const char* ASCII_ART = R"(
                  ###                       # ###       #######
                   ###                    /  /###     /       ###
                    ##                   /  /  ###   /         ##
                    ##                  /  ##   ###  ##        #
                    ##                 /  ###    ###  ###
   /###     /###    ##  ##   ####     ##   ##     ## ## ###
  / ###  / / ###  / ##   ##    ###  / ##   ##     ##  ### ###
 /   ###/ /   ###/  ##   ##     ###/  ##   ##     ##    ### ###
##    ## ##    ##   ##   ##      ##   ##   ##     ##      ### /##
##    ## ##    ##   ##   ##      ##   ##   ##     ##        #/ /##
##    ## ##    ##   ##   ##      ##    ##  ##     ##         #/ ##
##    ## ##    ##   ##   ##      ##     ## #      /           # /
##    ## ##    ##   ##   ##      ##      ###     /  /##        /
#######   ######    ### / #########       ######/  /  ########/
######     ####      ##/    #### ###        ###   /     #####
##                                ###             |
##                         #####   ###             \)
##                       /#######  /#
 ##                     /      ###/
)";

constexpr std::string_view CREDITS = "...by Benjamin Taylor";

const std::vector<char> bootChars = {
    ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
    'z', '.', ',', '-', '_', '=', '/', '\\', '|', '(', ')', '+', '*', '?', '#', '@'};

void Boot::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    
    // Parse ASCII art into a 2D array (moved outside - should be static or class member)
    static std::vector<std::string> artLines;
    static std::vector<std::vector<int>> artCharIndices;
    static std::vector<std::vector<bool>> revealed;
    static int artHeight = 0, artWidth = 0, artStartY = 0, artStartX = 0;
    static bool initialized = false;
    
    if (!initialized) {
        std::string artStr = ASCII_ART;
        std::istringstream iss(artStr);
        std::string line;
        while (std::getline(iss, line)) {
            if (!line.empty()) {
                artLines.push_back(line);
            }
        }
        
        artHeight = artLines.size();
        artWidth = 0;
        for (const auto& artLine : artLines) {
            artWidth = std::max(artWidth, static_cast<int>(artLine.length()));
        }
        
        // Calculate center position for ASCII art
        artStartY = (winHeight - artHeight) / 2;
        artStartX = (winWidth - artWidth) / 2;
        
        // Pre-calculate ASCII art character indices for performance
        artCharIndices.resize(artHeight);
        for (int y = 0; y < artHeight; ++y) {
            artCharIndices[y].resize(artWidth, 0);  // Default to space (index 0)
            for (int x = 0; x < artWidth && x < static_cast<int>(artLines[y].length()); ++x) {
                char artChar = artLines[y][x];
                // Find the index of the art character in bootChars
                for (size_t i = 0; i < bootChars.size(); ++i) {
                    if (bootChars[i] == artChar) {
                        artCharIndices[y][x] = i;
                        break;
                    }
                }
            }
        }
        
        // Initialize revealed matrix once
        revealed.resize(winHeight, std::vector<bool>(winWidth, false));
        initialized = true;
    }
    
    // Convert bootChars to array for faster access
    static const char bootCharsArray[] = {
        ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
        'z', '.', ',', '-', '_', '=', '/', '\\', '|', '(', ')', '+', '*', '?', '#', '@'
    };
    static constexpr int bootCharsSize = sizeof(bootCharsArray) / sizeof(bootCharsArray[0]);
    
    float t = 0.0f;
    float dt = 0.09f;  // Original speed
    float freq1 = 0.13f, freq2 = 0.09f, freq3 = 0.07f;
    float amp1 = 1.7f, amp2 = 1.2f, amp3 = 0.8f;
    
    // Pre-calculate some constants
    const float invWinWidth = 1.0f / winWidth;
    const float invWinHeight = 1.0f / winHeight;
    const float normScale = 6.0f;  // 3.0f * 2.0f
    const float normScaleY = 3.0f; // 1.5f * 2.0f
    const int maxBootCharIdx = bootCharsSize - 1;

    for (int frame = 0; frame < FRAME_COUNT; ++frame, t += dt) {
        // Calculate progress for a single wave that reaches max and fades
        float progress = static_cast<float>(frame) / (FRAME_COUNT - 1);
        float ease = 1.0f;
        
        if (progress < 0.4f) {
            ease = easeInQuad(progress / 0.4f);
        } else if (progress > 0.6f) {
            ease = 1.0f - easeOutQuad((progress - 0.6f) / 0.4f);
        }
        
        // Pre-calculate time-based wave components
        const float t1 = t;
        const float t2 = t * 1.2f;
        const float t3 = t * 0.7f;
        const float t4 = t * 1.5f;
        
        werase(context.window);
        for (int y = 0; y < winHeight; ++y) {
            // Pre-calculate y-dependent values
            const float ny = (2.0f * y * invWinHeight - 1.0f) * 1.5f;
            const float cosComponent = std::cos(ny * freq2 - t2) * amp2;
            
            for (int x = 0; x < winWidth; ++x) {
                // Normalized coordinates (optimized)
                const float nx = (2.0f * x * invWinWidth - 1.0f) * 3.0f;
                
                // Compose several sine/cosine waves (reduced calculations)
                float v = std::sin(nx * freq1 + t1) * amp1;
                v += cosComponent;  // Pre-calculated
                v += std::sin((nx + ny) * freq3 + t3) * amp3;
                
                // Radial ripple (optimized sqrt)
                const float r2 = nx * nx + ny * ny;  // r squared
                v += std::sin(r2 * 1.25f - t4) * 1.1f;  // Use r^2 * adjusted freq instead of sqrt(r^2)
                
                // Normalize to [0, 1] and apply easing (optimized tanh)
                // Using fast tanh approximation: tanh(x) ≈ x / (1 + |x|) for reasonable range
                const float absV = std::abs(v);
                float norm = (v / (1.0f + absV) + 1.0f) * 0.5f;
                norm *= ease * 1.5f;  // Combined multiplication
                
                // Calculate current wave character index (optimized)
                int waveIdx = static_cast<int>(norm * maxBootCharIdx);
                waveIdx = (waveIdx > maxBootCharIdx) ? maxBootCharIdx : waveIdx;  // Branchless clamp
                
                char displayChar = ' ';
                
                // Check if we're in the ASCII art area
                const int artY = y - artStartY;
                const int artX = x - artStartX;
                
                if (artY >= 0 && artY < artHeight && artX >= 0 && artX < artWidth) {
                    // Get the pre-calculated ASCII art character index
                    const int artCharIdx = artCharIndices[artY][artX];
                    
                    // Check if wave intensity is higher than the art character index
                    if (waveIdx > artCharIdx) {
                        revealed[y][x] = true;  // Mark as revealed
                    }
                    
                    if (revealed[y][x] && waveIdx < artCharIdx) {
                        // Show the original ASCII art character
                        if (artX < static_cast<int>(artLines[artY].length())) {
                            displayChar = artLines[artY][artX];
                        } else {
                            displayChar = ' ';
                        }
                    } else {
                        // Show the wave
                        displayChar = bootCharsArray[waveIdx];
                    }
                } else {
                    // Outside art area, just show wave
                    displayChar = bootCharsArray[waveIdx];
                }
                
                mvwaddch(context.window, y, x, displayChar);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(400));

    // Extra character at the end for the blinking cursor
    int creditsWidth = CREDITS.size() + 1;

    // Placing the credits just below the art
    int creditsRightX = artStartX + artWidth + 1;
    int creditsLeftX = creditsRightX - creditsWidth;
    int creditsLeftY = artStartY + artHeight + 2;

    wmove(context.window, creditsLeftY, creditsLeftX);
    curs_set(2);

    std::uniform_int_distribution<> distr(50, 250);    

    for (char c : CREDITS) {
        wrefresh(context.window);
        waddch(context.window, c);
        std::this_thread::sleep_for(std::chrono::milliseconds(distr(context.rng)));
    }

    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
