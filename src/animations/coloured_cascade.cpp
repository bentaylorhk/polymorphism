/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "coloured_cascade.h"

#include <ncurses.h>

#include <algorithm>
#include <numeric>
#include <string>
#include <thread>

#include "../common.h"
#include "../util/colours.h"

constexpr int DRAW_SPEED = 50;

void ColouredCascade::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    int wordLen = polyphonic.size();
    int numFullCols = winWidth / wordLen;
    int lastColWidth = winWidth % wordLen;

    // Create subwindows for each column
    std::vector<WINDOW *> subwins;
    for (int col = 0; col < numFullCols; ++col) {
        int x = col * wordLen;
        subwins.push_back(derwin(context.window, winHeight, wordLen, 0, x));
    }

    // Truncated window at the end
    if (lastColWidth > 0) {
        subwins.push_back(derwin(context.window, winHeight, lastColWidth, 0,
                                 numFullCols * wordLen));
    }

    // Prepare row indices and shuffle for random order
    std::vector<int> winOrder(subwins.size());
    std::iota(winOrder.begin(), winOrder.end(), 0);
    std::shuffle(winOrder.begin(), winOrder.end(), context.rng);

    // Pick a random gradient
    Gradient gradient = getRandomGradient(context.rng);
    std::uniform_int_distribution<int> colourDist(0, GRADIENT_LENGTH - 1);

    // Fill each window in random order, preserving previous text, with colour
    for (auto winIdx : winOrder) {
        int width = (winIdx == numFullCols && lastColWidth > 0) ? lastColWidth
                                                                : wordLen;
        int gradIdx = colourDist(context.rng);
        int colourPair = getColourIndex(gradient, gradIdx);
        wattron(subwins[winIdx], COLOR_PAIR(colourPair));
        for (int row = 0; row < winHeight; ++row) {
            for (int i = 0; i < width; ++i) {
                mvwaddch(subwins[winIdx], row, i, polyphonic[i]);
            }
            wrefresh(subwins[winIdx]);
            std::this_thread::sleep_for(std::chrono::milliseconds(DRAW_SPEED));
        }
        wattroff(subwins[winIdx], COLOR_PAIR(colourPair));
    }

    // Wait before clearing
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::shuffle(winOrder.begin(), winOrder.end(), context.rng);

    // Clear each window in random order, no colour needed
    for (auto winIdx : winOrder) {
        int width = (winIdx == numFullCols && lastColWidth > 0) ? lastColWidth
                                                                : wordLen;
        for (int row = 0; row < winHeight; ++row) {
            for (int i = 0; i < width; ++i) {
                mvwaddch(subwins[winIdx], row, i, ' ');
            }
            wrefresh(subwins[winIdx]);
            std::this_thread::sleep_for(std::chrono::milliseconds(DRAW_SPEED));
        }
    }

    // Cleanup
    for (auto *w : subwins)
        delwin(w);
}
