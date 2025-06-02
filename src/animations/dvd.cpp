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
    std::random_device rd;
    std::mt19937 g(rd());

    // Pick a random initial direction
    if (g() % 2) {
        dx = -dx;
    }
    if (g() % 2) {
        dy = -dy;
    }

    int trailLength = 4;
    std::deque<std::pair<int, int>> trail;
    trail.push_back({x, y});
    for (int i = 0; i < steps; ++i) {
        werase(paddedWindow);
        // Draw trail from oldest to newest, so newest is on top
        for (size_t t = 0; t < trail.size(); ++t) {
            int tx = trail[t].first;
            int ty = trail[t].second;

            // TODO: Better colours, maybe even fade the trail out
            wattron(paddedWindow, COLOR_PAIR(t));
            mvwprintw(paddedWindow, ty, tx, "%s", polyphonic.c_str());
            wattroff(paddedWindow, COLOR_PAIR(t));
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
