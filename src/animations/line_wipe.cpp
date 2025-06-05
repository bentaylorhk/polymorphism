/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "line_wipe.h"

#include <ncurses.h>

#include <random>
#include <thread>

void LineWipe::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    std::bernoulli_distribution dist(0.5);
    bool topToBottom = dist(context.rng);

    auto writeLine = [&](int y, char c) {
        for (int x = 0; x < winWidth; ++x) {
            mvwaddch(context.window, y, x, c);
        }
    };

    if (topToBottom) {
        for (int y = 0; y < winHeight; ++y) {
            writeLine(y, '=');
            wrefresh(context.window);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
            writeLine(y, ' ');
        }
    } else {
        for (int y = winHeight - 1; y >= 0; --y) {
            writeLine(y, '=');
            wrefresh(context.window);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
            writeLine(y, ' ');
        }
    }
    wrefresh(context.window);
}
