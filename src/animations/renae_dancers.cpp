/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "renae_dancers.h"

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

void RenaeDancers::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    werase(context.window);

    // Load dancers
    AsciiArt dancer1 = loadAsciiArt("renae/dancer1.txt");
    AsciiArt dancer2 = loadAsciiArt("renae/dancer2.txt");
    AsciiArt dancer3 = loadAsciiArt("renae/dancer3.txt");
    std::vector<AsciiArt> dancers = {dancer1, dancer2, dancer3};

    Gradient gradient = getRandomGradient(context.rng);

    // Assign a color for each dancer (use 3 distinct color pairs)
    std::vector<int> dancerColors = {getColourIndex(gradient, 0),
                                     getColourIndex(gradient, 2),
                                     getColourIndex(gradient, 4)};

    // Calculate each dancer's width and height
    std::vector<int> dancerWidths, dancerHeights;
    for (const auto &d : dancers) {
        dancerWidths.push_back(d.getWidth());
        dancerHeights.push_back(d.getHeight());
    }

    // Each dancer has its own direction, speed, and vertical center
    std::vector<int> directions = {1, -1,
                                   1};  // 1: left->right, -1: right->left
    std::uniform_real_distribution<float> speedDist(0.7f, 1.5f);
    std::vector<float> speeds;
    for (size_t i = 0; i < dancers.size(); ++i) {
        speeds.push_back(speedDist(context.rng));
    }
    // Vertically center each dancer independently
    std::vector<int> dancerTops;
    for (size_t i = 0; i < dancers.size(); ++i) {
        int top = (winHeight - dancerHeights[i]) / 2;
        dancerTops.push_back(std::max(0, top));
    }

    // Animation: scroll each dancer independently
    std::vector<float> positions;
    for (size_t i = 0; i < dancers.size(); ++i) {
        if (directions[i] == 1) {
            positions.push_back(-dancerWidths[i]);
        } else {
            positions.push_back(winWidth);
        }
    }

    // int steps = 600;
    // for (int frame = 0; frame < steps; ++frame) {
    // Fore each dancer,
    // Just iterate over a starting X axis, which then I could modulo the
    // width (plus a bit maybe) to see the dancers scroll through (I guess
    // modulo screen size plus dancer width).
    //}

    // Create a pad for each dancer
    std::vector<WINDOW *> dancerPads;
    for (size_t i = 0; i < dancers.size(); ++i) {
        WINDOW *pad = newpad(dancerHeights[i], dancerWidths[i]);
        for (int y = 0; y < dancerHeights[i]; ++y) {
            for (int x = 0; x < dancerWidths[i]; ++x) {
                char c = dancers[i].getChar(x, y);
                if (c != ' ') {
                    wattron(pad, COLOR_PAIR(dancerColors[i]));
                    mvwaddch(pad, y, x, c);
                    wattroff(pad, COLOR_PAIR(dancerColors[i]));
                } else {
                    mvwaddch(pad, y, x, ' ');
                }
            }
        }
        dancerPads.push_back(pad);
    }

    int steps = 600;
    for (int frame = 0; frame < steps; ++frame) {
        werase(context.window);
        for (size_t i = 0; i < dancers.size(); ++i) {
            // Update position
            positions[i] += speeds[i] * directions[i];
            int x = (int)positions[i];
            int y = dancerTops[i];
            int padX = 0;
            int padWidth = dancerWidths[i];
            int screenX = x;
            // Clip if offscreen
            if (screenX < 0) {
                padX = -screenX;
                padWidth -= padX;
                screenX = 0;
            }
            if (screenX + padWidth > winWidth) {
                padWidth = winWidth - screenX;
            }
            if (padWidth > 0 && screenX < winWidth && y >= 0 && y < winHeight) {
                prefresh(dancerPads[i], 0, padX,  // pad y, pad x
                         y, screenX,              // screen y, screen x
                         y + dancerHeights[i] - 1,
                         screenX + padWidth - 1  // screen y2, screen x2
                );
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
    }
    for (auto pad : dancerPads) {
        delwin(pad);
    }
}
