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
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    // Load renae logo
    AsciiArt renae = loadAsciiArt("renae/padded-logo.txt");
    int renaeHeight = renae.getHeight();
    int renaeWidth = renae.getWidth();
    int renaeY = (winHeight - renaeHeight) / 2;  // Centered vertically
    int renaeX = (winWidth - renaeWidth) / 2;

    int x = (winWidth - context.wordLen()) / 2;
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

    int grey = getGreyColourIndex();

    struct Poly {
        int x, y, dx, dy;
        Gradient gradient;
        std::deque<std::pair<int, int>> trail;
    };
    int trailLength = GRADIENT_LENGTH;
    std::vector<Poly> polys;
    std::uniform_int_distribution<int> xDist(5,
                                             winWidth - context.wordLen() - 5);
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

    // Animate renae logo fizzing in before DVD starts
    werase(context.window);
    int maxRadius = std::max(renaeWidth, renaeHeight);
    int renaeDrawTime = MS_PER_DOUBLE_BEAT / maxRadius;
    wattron(context.window, COLOR_PAIR(grey));
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
    wattroff(context.window, COLOR_PAIR(grey));

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));

    // Dynamically set start delays for each poly
    std::vector<int> polyStartDelays(polys.size());
    for (size_t i = 0; i < polys.size(); ++i) {
        // One spawn per 4 bars (given eigth wait is used)
        polyStartDelays[i] = i * 8 * 4;
    }

    int steps = 600;
    for (int i = 0; i < steps; ++i) {
        werase(context.window);

        // Render renae logo first (background)
        wattron(context.window, COLOR_PAIR(grey));
        for (int y = 0; y < renaeHeight; ++y) {
            for (int x = 0; x < renaeWidth; ++x) {
                char c = renae.getChar(x, y);
                if (c == '*') {
                    mvwaddch(context.window, renaeY + y, renaeX + x, ' ');
                } else if (c != ' ') {
                    mvwaddch(context.window, renaeY + y, renaeX + x, c);
                }
            }
        }
        wattroff(context.window, COLOR_PAIR(grey));

        // Render DVD polys on top
        for (int pidx = 0; pidx < polys.size(); ++pidx) {
            if (i < polyStartDelays[pidx])
                continue;
            Poly &p = polys[pidx];
            for (size_t t = 0; t < p.trail.size(); ++t) {
                int tx = p.trail[t].first;
                int ty = p.trail[t].second;
                // Reverse the gradient: oldest = 0, newest = GRADIENT_LENGTH-1
                int colourIndex =
                    getColourIndex(p.gradient, (int)p.trail.size() - 1 - t);
                wattron(context.window, COLOR_PAIR(colourIndex) | A_BOLD);
                mvwprintw(context.window, ty, tx, "%s", context.word.c_str());
                wattroff(context.window, COLOR_PAIR(colourIndex) | A_BOLD);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
        for (int pidx = 0; pidx < polys.size(); ++pidx) {
            if (i < polyStartDelays[pidx])
                continue;
            Poly &p = polys[pidx];
            p.x += p.dx;
            p.y += p.dy;
            if (p.x <= 0 || p.x + context.wordLen() >= winWidth)
                p.dx = -p.dx;
            if (p.y <= 0 || p.y >= winHeight - 1)
                p.dy = -p.dy;
            if (p.x < 0)
                p.x = 0;
            if (p.x + context.wordLen() > winWidth)
                p.x = winWidth - context.wordLen();
            if (p.y < 0)
                p.y = 0;
            if (p.y >= winHeight)
                p.y = winHeight - 1;
            p.trail.push_back({p.x, p.y});
            if ((int)p.trail.size() > trailLength)
                p.trail.pop_front();
        }
    }
}
