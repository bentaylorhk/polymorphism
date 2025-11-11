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

    AsciiArt polyphonic = loadAsciiArt("polyphonic.txt");
    int artHeight = polyphonic.getHeight();
    int artWidth = polyphonic.getWidth();
    int vertOffset = (winHeight - artHeight) / 2;

    int colour = getRandomColourIndex(context.rng);

    int repetitions = 2;
    int gap = 2;
    int totalArtWidth = repetitions * (artWidth + gap);
    int totalFrames = winWidth + totalArtWidth;
    for (int frame = -winWidth; frame < totalArtWidth; ++frame) {
        werase(context.window);
        wattron(context.window, COLOR_PAIR(colour));
        for (int y = 0; y < artHeight && vertOffset + y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                int artPos = frame + x;
                if (artPos < 0 || artPos >= totalArtWidth)
                    continue;
                int rep = artPos / (artWidth + gap);
                int offset = artPos % (artWidth + gap);
                if (offset < artWidth) {
                    char c = polyphonic.getChar(offset, y);
                    if (c != ' ') {
                        mvwaddch(context.window, vertOffset + y, x, c);
                    }
                }  // else: gap, leave blank
            }
        }
        wattroff(context.window, COLOR_PAIR(colour));
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }
}
