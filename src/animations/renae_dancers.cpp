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
    static std::vector<std::string> dancer1 =
        loadAsciiArt("ascii-art/renae/dancer1.txt");
    static std::vector<std::string> dancer2 =
        loadAsciiArt("ascii-art/renae/dancer2.txt");
    static std::vector<std::string> dancer3 =
        loadAsciiArt("ascii-art/renae/dancer3.txt");
    std::vector<std::vector<std::string>> dancers = {dancer1, dancer2, dancer3};

    Gradient gradient = getRandomGradient(context.rng);

    // Assign a color for each dancer (use 3 distinct color pairs)
    std::vector<int> dancerColors = {getColourIndex(gradient, 0),
                                     getColourIndex(gradient, 2),
                                     getColourIndex(gradient, 4)};

    // Calculate each dancer's width and height
    std::vector<int> dancerWidths, dancerHeights;
    for (const auto &d : dancers) {
        int w = 0;
        for (const auto &l : d)
            w = std::max(w, (int)l.size());
        dancerWidths.push_back(w);
        dancerHeights.push_back(d.size());
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

    int steps = 600;
    for (int frame = 0; frame < steps; ++frame) {
        werase(context.window);
        for (size_t i = 0; i < dancers.size(); ++i) {
            // Update position
            positions[i] += speeds[i] * directions[i];
            int x = (int)positions[i];
            int y = dancerTops[i];
            wattron(context.window, COLOR_PAIR(dancerColors[i]));
            for (size_t j = 0; j < dancers[i].size(); ++j) {
                int drawX = x;
                if (drawX < 0)
                    drawX = 0;
                if (drawX < winWidth && y + (int)j < winHeight &&
                    drawX < winWidth) {
                    std::string line = dancers[i][j];
                    if (drawX + (int)line.size() > winWidth) {
                        line = line.substr(0, winWidth - drawX);
                    }
                    if (drawX < winWidth && !line.empty())
                        mvwprintw(context.window, y + j, drawX, "%s",
                                  line.c_str());
                }
            }
            wattroff(context.window, COLOR_PAIR(dancerColors[i]));
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
    }
}
