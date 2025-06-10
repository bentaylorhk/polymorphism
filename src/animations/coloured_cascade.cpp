/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "coloured_cascade.h"

#include <ncurses.h>

#include <algorithm>
#include <numeric>
#include <thread>

#include "../util/colours.h"
#include "../util/common.h"

void ColouredCascade::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    int numFullCols = winWidth / context.wordLen();
    int lastColWidth = winWidth % context.wordLen();

    // Create subwindows for each column
    std::vector<WINDOW *> subwins;
    for (int col = 0; col < numFullCols; ++col) {
        int x = col * context.wordLen();
        subwins.push_back(derwin(context.window, winHeight, context.wordLen(), 0, x));
    }

    // Truncated window at the end
    if (lastColWidth > 0) {
        subwins.push_back(derwin(context.window, winHeight, lastColWidth, 0,
                                 numFullCols * context.wordLen()));
    }

    // Clear all subwindows to prevent inherited content/colours
    for (auto *w : subwins) {
        wclear(w);
        wrefresh(w);
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
                                                                : context.wordLen();
        int gradIdx = colourDist(context.rng);
        int colourPair = getColourIndex(gradient, gradIdx);
        wattron(subwins[winIdx], COLOR_PAIR(colourPair));
        for (int row = 0; row < winHeight; ++row) {
            for (int i = 0; i < width; ++i) {
                mvwaddch(subwins[winIdx], row, i, context.word[i]);
            }
            wrefresh(subwins[winIdx]);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
        }
        wattroff(subwins[winIdx], COLOR_PAIR(colourPair));
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_BEAT + MS_PER_EIGHTH_BEAT));
    }

    // Wait before clearing
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_QUADRUPLE_BEAT));

    std::shuffle(winOrder.begin(), winOrder.end(), context.rng);

    // Clear each window in random order, no colour needed
    for (auto winIdx : winOrder) {
        int width = (winIdx == numFullCols && lastColWidth > 0) ? lastColWidth
                                                                : context.wordLen();
        for (int row = 0; row < winHeight; ++row) {
            for (int i = 0; i < width; ++i) {
                mvwaddch(subwins[winIdx], row, i, ' ');
            }
            wrefresh(subwins[winIdx]);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
        }
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_BEAT + MS_PER_EIGHTH_BEAT));
    }

    // Cleanup
    for (auto *w : subwins)
        delwin(w);
}
