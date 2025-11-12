/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "renae_dancers.h"

#include <ncurses.h>

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../util/colours.h"
#include "../util/common.h"

void RenaeDancers::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    // Load dancers
    AsciiArt dancer1 = loadAsciiArt("renae/dancer1.txt");
    AsciiArt dancer2 = loadAsciiArt("renae/dancer2.txt");
    AsciiArt dancer3 = loadAsciiArt("renae/dancer3.txt");
    std::vector<AsciiArt> dancers = {dancer1, dancer2, dancer3};

    Gradient gradient = getRandomGradient(context.rng);

    // Generate random x positions for dancer placement
    std::uniform_int_distribution<int> xDist(0, winWidth - 1);

    int numDancers = 48;

    // Clear screen once at the start
    werase(context.window);

    // Load and render renae logo with ripple reveal
    AsciiArt renae = loadAsciiArt("renae/padded-logo.txt");
    int renaeHeight = renae.getHeight();
    int renaeWidth = renae.getWidth();
    int renaeY = (winHeight - renaeHeight) / 2;  // Centered vertically
    int renaeX = (winWidth - renaeWidth) / 2;

    // Ripple reveal animation for renae logo
    int maxRadius = std::max(renaeWidth, renaeHeight);
    int renaeDrawTime = MS_PER_DOUBLE_BEAT / maxRadius;
    for (int radius = 0; radius <= maxRadius; ++radius) {
        for (int y = 0; y < renaeHeight; ++y) {
            for (int x = 0; x < renaeWidth; ++x) {
                int dx = x - renaeWidth / 2;
                int dy = y - renaeHeight / 2;
                int dist = static_cast<int>(std::sqrt(dx * dx + dy * dy));
                if (dist <= radius) {
                    char c = renae.getChar(x, y);
                    if (c == '*') {
                        mvwaddch(context.window, renaeY + y, renaeX + x, ' ');
                    } else if (c != ' ') {
                        mvwaddch(context.window, renaeY + y, renaeX + x, c);
                    }
                }
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(renaeDrawTime));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));

    // Animate placing dancers one by one (they overlay on top)
    for (int i = 0; i < numDancers; ++i) {
        // Cycle through dancer types
        int dancerType = i % dancers.size();
        const AsciiArt &dancer = dancers[dancerType];

        // Cycle through gradient colors
        int colorIndex = getColourIndex(gradient, i % GRADIENT_LENGTH);

        int dancerDrawTime = MS_PER_HALF_BEAT / dancer.getHeight();

        // Generate consistent random position for each dancer index
        std::mt19937 tempRng(
            i);  // Use index as seed for consistent positioning
        std::uniform_int_distribution<int> tempXDist(0, winWidth - 1);
        int startX = tempXDist(tempRng);

        int startY = (winHeight - dancer.getHeight()) / 2;

        wattron(context.window, COLOR_PAIR(colorIndex));

        // Draw dancer from bottom to top, row by row
        for (int y = dancer.getHeight() - 1; y >= 0; --y) {
            for (int x = 0; x < dancer.getWidth(); ++x) {
                char c = dancer.getChar(x, y);
                if (c != ' ') {
                    int screenY = startY + y;
                    int screenX =
                        (startX + x) %
                        (winWidth + 12);  // Wrap around with slight overhang
                    if (screenY >= 0 && screenY < winHeight &&
                        screenX < winWidth) {
                        mvwaddch(context.window, screenY, screenX, c);
                    }
                }
            }
            wrefresh(context.window);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(dancerDrawTime));
        }

        wattroff(context.window, COLOR_PAIR(colorIndex));
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_HALF_BEAT));
    }

    // Hold final frame
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));
}
