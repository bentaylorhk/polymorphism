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
    
    // Parse ASCII art into a 2D array
    std::vector<std::string> artLines;
    std::string artStr = ASCII_ART;
    std::istringstream iss(artStr);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            artLines.push_back(line);
        }
    }
    
    int artHeight = artLines.size();
    int artWidth = 0;
    for (const auto& artLine : artLines) {
        artWidth = std::max(artWidth, static_cast<int>(artLine.length()));
    }
    
    // Calculate center position for ASCII art
    int artStartY = (winHeight - artHeight) / 2;
    int artStartX = (winWidth - artWidth) / 2;
    
    // Pre-calculate ASCII art character indices for performance
    std::vector<std::vector<int>> artCharIndices(artHeight);
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
    
    // Track which positions have been revealed
    std::vector<std::vector<bool>> revealed(winHeight, std::vector<bool>(winWidth, false));
    
    float t = 0.0f;
    float dt = 0.09f;  // Original speed
    float freq1 = 0.13f, freq2 = 0.09f, freq3 = 0.07f;
    float amp1 = 1.7f, amp2 = 1.2f, amp3 = 0.8f;

    for (int frame = 0; frame < FRAME_COUNT; ++frame, t += dt) {
        // Calculate progress for a single wave that reaches max and fades
        float progress = static_cast<float>(frame) / (FRAME_COUNT - 1);
        float ease = 1.0f;
        
        if (progress < 0.4f) {
            ease = easeInQuad(progress / 0.3f);
        } else if (progress > 0.6f) {
            ease = 1.0f - easeOutQuad((progress - 0.7f) / 0.3f);
        }
        
        werase(context.window);
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                // Normalized coordinates
                float nx = (2.0f * x / winWidth - 1.0f) * 3.0f;
                float ny = (2.0f * y / winHeight - 1.0f) * 1.5f;
                
                // Compose several sine/cosine waves
                float v = 0.0f;
                v += std::sin(nx * freq1 + t) * amp1;
                v += std::cos(ny * freq2 - t * 1.2f) * amp2;
                v += std::sin((nx + ny) * freq3 + t * 0.7f) * amp3;
                
                // Radial ripple
                float r = std::sqrt(nx * nx + ny * ny);
                v += std::sin(r * 2.5f - t * 1.5f) * 1.1f;
                
                // Normalize to [0, 1] and apply easing
                float norm = (std::tanh(v) + 1.0f) / 2.0f;
                norm *= ease;
                
                // Allow the normalized value to exceed 1 by multiplying by 2
                norm *= 1.1f;
                
                // Calculate current wave character index (scaled to max)
                int waveIdx = static_cast<int>(norm * (bootChars.size() - 1));
                
                // Clamp index values above the max to the max index
                waveIdx = std::min(waveIdx, static_cast<int>(bootChars.size() - 1));
                
                char displayChar = ' ';
                
                // Check if we're in the ASCII art area
                int artY = y - artStartY;
                int artX = x - artStartX;
                
                if (artY >= 0 && artY < artHeight && artX >= 0 && artX < artWidth) {
                    // Get the pre-calculated ASCII art character index
                    int artCharIdx = artCharIndices[artY][artX];
                    
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
                        displayChar = bootChars[waveIdx];
                    }
                } else {
                    // Outside art area, just show wave
                    displayChar = bootChars[waveIdx];
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
