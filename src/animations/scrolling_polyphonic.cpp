/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "scrolling_polyphonic.h"

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

void ScrollingPolyphonic::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    std::vector<std::string> polyphonicArt =
        loadAsciiArt("ascii-art/polyphonic.txt");

    int colour = getRandomColourIndex(context.rng);

    // First move the art in from left to right, then rotate it for a while.

    int steps = 1000;
    for (int frame = 0; frame < steps; frame++) {
        werase(context.window);
        for (int x = 0; x < winWidth; ++x) {
            for (int y = 0; y < winHeight; ++y) {
                // Calculate which character from the art to draw at (x, y)
                int artX = (x + frame) % polyphonicArt[0].size();

                if (artX >= 0 && artX < polyphonicArt[0].size()) {
                    char c = polyphonicArt[y][artX];
                    if (c != ' ') {
                        wattron(context.window, COLOR_PAIR(colour));
                        mvwaddch(context.window, y, x, c);
                        wattroff(context.window, COLOR_PAIR(colour));
                    }
                }
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }
}
