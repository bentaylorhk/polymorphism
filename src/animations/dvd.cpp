/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "dvd.h"

#include <ncurses.h>

#include <chrono>
#include <random>
#include <thread>

#include "../common.h"

void DVD::drawFrame(const AnimationContext &context) {
    int outerWinHeight, winWidth;
    context.getDimensions(outerWinHeight, winWidth);

    int topPadding = 1;
    int winHeight = outerWinHeight - topPadding;

    WINDOW *paddedWindow =
        derwin(context.window, winHeight, winWidth, topPadding, 0);

    const int wordLen = polyphonic.size();
    int x = (winWidth - wordLen) / 2;
    int y = winHeight / 2;
    int dx = 1;
    int dy = 1;
    int steps = 200;  // total bounces
    std::random_device rd;
    std::mt19937 g(rd());

    // Pick a random initial direction
    if (g() % 2) {
        dx = -dx;
    }
    if (g() % 2) {
        dy = -dy;
    }

    for (int i = 0; i < steps; ++i) {
        werase(paddedWindow);
        mvwprintw(paddedWindow, y, x, "%s", polyphonic.c_str());
        wrefresh(paddedWindow);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME / 2));
        x += dx;
        y += dy;
        if (x <= 0 || x + wordLen >= winWidth)
            dx = -dx;
        if (y <= 0 || y >= winHeight - 1)
            dy = -dy;
        // Clamp to bounds
        if (x < 0)
            x = 0;
        if (x + wordLen > winWidth)
            x = winWidth - wordLen;
        if (y < 0)
            y = 0;
        if (y >= winHeight)
            y = winHeight - 1;
    }

    delwin(paddedWindow);
}
