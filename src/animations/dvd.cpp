/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "dvd.h"

#include <ncurses.h>

#include <chrono>
#include <deque>
#include <random>
#include <thread>

#include "../common.h"
#include "../util/colours.h"

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
    int steps = 350;  // total bounces

    // 50/50 random inital directions
    std::bernoulli_distribution dist(0.5);
    if (dist(context.rng)) {
        dx = -dx;
    }
    if (dist(context.rng)) {
        dy = -dy;
    }

    Gradient gradient = getRandomGradient(context.rng);

    int trailLength = GRADIENT_LENGTH;
    std::deque<std::pair<int, int>> trail;
    trail.push_back({x, y});
    for (int i = 0; i < steps; ++i) {
        werase(paddedWindow);
        // Draw trail from oldest to newest, so newest is on top
        for (size_t t = 0; t < trail.size(); ++t) {
            int tx = trail[t].first;
            int ty = trail[t].second;

            int colourIndex = getColourIndex(gradient, t);

            wattron(paddedWindow, COLOR_PAIR(colourIndex) | A_BOLD);
            mvwprintw(paddedWindow, ty, tx, "%s", polyphonic.c_str());
            wattroff(paddedWindow, COLOR_PAIR(colourIndex) | A_BOLD);
        }
        wrefresh(paddedWindow);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME / 4));

        // Get new position of front word
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

        trail.push_back({x, y});
        if ((int)trail.size() > trailLength) {
            trail.pop_front();
        }
    }

    delwin(paddedWindow);
}
