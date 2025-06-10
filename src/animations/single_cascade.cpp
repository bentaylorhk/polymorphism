/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "single_cascade.h"

#include <ncurses.h>

#include <algorithm>
#include <chrono>
#include <thread>

constexpr int CASCADE_TIME = MS_PER_EIGHTH_BEAT;

void SingleCascade::drawFrame(const AnimationContext &context) {
    int winHeight, outerWinWidth;
    context.getDimensions(winHeight, outerWinWidth);

    // Padding outer columns
    int padding = 4;
    int winWidth = outerWinWidth - (padding * 2);

    WINDOW *paddedWindow =
        derwin(context.window, winHeight, winWidth, 0, padding);

    wclear(paddedWindow);
    wrefresh(paddedWindow);

    int colWidth = winWidth / context.wordLen();
    int colRem = winWidth % context.wordLen();
    std::vector<WINDOW *> subWins;
    int x = 0;
    for (int i = 0; i < context.wordLen(); ++i) {
        int w = colWidth;
        subWins.push_back(derwin(paddedWindow, winHeight, w, 0, x));
        x += w;
    }

    int middleRow = winHeight / 2;
    int trailLength = 5;

    std::vector<int> indices(context.wordLen());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), context.rng);
    for (int idx : indices) {
        for (int row = 0; row < middleRow + trailLength; row++) {
            int eraseIdx = row - trailLength;
            if (row <= middleRow) {
                mvwaddch(subWins[idx], row, (colWidth - 1) / 2,
                         context.word[idx]);
            }
            if (eraseIdx >= 0) {
                mvwaddch(subWins[idx], eraseIdx, (colWidth - 1) / 2, ' ');
            }
            wrefresh(subWins[idx]);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(CASCADE_TIME));
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_OCTUPLE_BEAT));

    std::shuffle(indices.begin(), indices.end(), context.rng);

    for (int idx : indices) {
        for (int row = middleRow; row < winHeight + trailLength; row++) {
            int eraseIdx = row - trailLength;
            if (row < winHeight) {
                mvwaddch(subWins[idx], row, (colWidth - 1) / 2,
                         context.word[idx]);
            }
            if (eraseIdx >= middleRow) {
                mvwaddch(subWins[idx], eraseIdx, (colWidth - 1) / 2, ' ');
            }
            wrefresh(subWins[idx]);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(CASCADE_TIME));
        }
    }

    for (auto w : subWins) {
        overwrite(w, paddedWindow);
        delwin(w);
    }
    overwrite(paddedWindow, context.window);
    delwin(paddedWindow);
}
