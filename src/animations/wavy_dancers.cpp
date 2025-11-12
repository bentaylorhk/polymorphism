/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "wavy_dancers.h"

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

void WavyDancers::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    std::string text = "POLYPHONIC x RENAE x ";
    int textLength = text.length();
    int grey = getGreyColourIndex();

    // Create offset for each alternating line with diagonal pattern
    std::vector<int> lineOffsets;
    for (int y = 0; y < winHeight; y += 2) {
        lineOffsets.push_back(y);  // Create diagonal/jagged start
    }

    int totalFrames = 600;
    for (int frame = 0; frame < totalFrames; ++frame) {
        werase(context.window);

        // Every 16 frames, randomly shift one line
        if (frame % 16 == 0 && !lineOffsets.empty()) {
            int randomLine = context.rng() % lineOffsets.size();
            int direction = (context.rng() % 2 == 0) ? -1 : 1;
            lineOffsets[randomLine] += direction;
        }

        // Draw text on alternating lines
        wattron(context.window, COLOR_PAIR(grey));
        for (int y = 0, lineIndex = 0; y < winHeight; y += 2, ++lineIndex) {
            if (lineIndex >= lineOffsets.size())
                break;

            int offset = lineOffsets[lineIndex];
            for (int x = 0; x < winWidth; ++x) {
                int textIndex =
                    ((offset + x) % textLength + textLength) % textLength;
                mvwaddch(context.window, y, x, text[textIndex]);
            }
        }
        wattroff(context.window, COLOR_PAIR(grey));

        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }
}
