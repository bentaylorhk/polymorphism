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

#include "../util/colours.h"
#include "../util/common.h"

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

    // 50/50 random inital directions
    std::bernoulli_distribution dist(0.5);
    if (dist(context.rng)) {
        dx = -dx;
    }
    if (dist(context.rng)) {
        dy = -dy;
    }

    struct Poly {
        int x, y, dx, dy;
        Gradient gradient;
        std::deque<std::pair<int, int>> trail;
    };
    int trailLength = GRADIENT_LENGTH;
    std::vector<Poly> polys;
    std::uniform_int_distribution<int> xDist(5, winWidth - wordLen - 5);
    std::uniform_int_distribution<int> yDist(5, winHeight - 1 - 5);
    std::vector<Gradient> gradients = getAllRandomGradients(context.rng);
    for (int i = 0; i < gradients.size(); ++i) {
        Poly p;
        p.x = xDist(context.rng);
        p.y = yDist(context.rng);
        p.dx = dist(context.rng) ? 1 : -1;
        p.dy = dist(context.rng) ? 1 : -1;
        p.gradient = gradients[i];
        p.trail.push_back({p.x, p.y});
        polys.push_back(p);
    }

    // Dynamically set start delays for each poly
    std::vector<int> polyStartDelays(polys.size());
    for (size_t i = 0; i < polys.size(); ++i) {
        polyStartDelays[i] = i * 50;
    }

    int steps = 500;  // total bounces
    for (int i = 0; i < steps; ++i) {
        werase(paddedWindow);
        for (int pidx = 0; pidx < polys.size(); ++pidx) {
            if (i < polyStartDelays[pidx])
                continue;
            Poly &p = polys[pidx];
            for (size_t t = 0; t < p.trail.size(); ++t) {
                int tx = p.trail[t].first;
                int ty = p.trail[t].second;
                int colourIndex = getColourIndex(p.gradient, t);
                wattron(paddedWindow, COLOR_PAIR(colourIndex) | A_BOLD);
                mvwprintw(paddedWindow, ty, tx, "%s", polyphonic.c_str());
                wattroff(paddedWindow, COLOR_PAIR(colourIndex) | A_BOLD);
            }
        }
        wrefresh(paddedWindow);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
        for (int pidx = 0; pidx < polys.size(); ++pidx) {
            if (i < polyStartDelays[pidx])
                continue;
            Poly &p = polys[pidx];
            p.x += p.dx;
            p.y += p.dy;
            if (p.x <= 0 || p.x + wordLen >= winWidth)
                p.dx = -p.dx;
            if (p.y <= 0 || p.y >= winHeight - 1)
                p.dy = -p.dy;
            if (p.x < 0)
                p.x = 0;
            if (p.x + wordLen > winWidth)
                p.x = winWidth - wordLen;
            if (p.y < 0)
                p.y = 0;
            if (p.y >= winHeight)
                p.y = winHeight - 1;
            p.trail.push_back({p.x, p.y});
            if ((int)p.trail.size() > trailLength)
                p.trail.pop_front();
        }
    }

    // Copy the final frame to the parent window before deleting paddedWindow
    overwrite(paddedWindow, context.window);
    delwin(paddedWindow);
}
