/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "vertical_wipe.h"

#include <ncurses.h>

#include <random>
#include <thread>

constexpr int WIPE_SPEED = MS_PER_SIXTEENTH_BEAT;

void VerticalWipe::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    std::bernoulli_distribution dist(0.5);
    bool topToBottom = dist(context.rng);

    if (topToBottom) {
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                mvwaddch(context.window, y, x, ' ');
            }
            wrefresh(context.window);
            std::this_thread::sleep_for(std::chrono::milliseconds(WIPE_SPEED));
        }
    } else {
        for (int y = winHeight - 1; y >= 0; --y) {
            for (int x = 0; x < winWidth; ++x) {
                mvwaddch(context.window, y, x, ' ');
            }
            wrefresh(context.window);
            std::this_thread::sleep_for(std::chrono::milliseconds(WIPE_SPEED));
        }
    }
}