/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "single_cascade.h"

#include <ncurses.h>

#include <algorithm>
#include <chrono>
#include <random>
#include <thread>

void SingleCascade::drawFrame(const AnimationContext &context) {
    int polyphonicLength = polyphonic.size();

    int winHeight, outerWinWidth;
    context.getDimensions(winHeight, outerWinWidth);

    // Padding outer columns
    int padding = 4;
    int winWidth = outerWinWidth - (padding * 2);

    WINDOW *paddedWindow =
        derwin(context.window, winHeight, winWidth, 0, padding);

    int col_width = winWidth / polyphonicLength;
    int col_rem = winWidth % polyphonicLength;
    std::vector<WINDOW *> subwins;
    int x = 0;
    for (int i = 0; i < polyphonicLength; ++i) {
        int w = col_width;
        subwins.push_back(derwin(paddedWindow, winHeight, w, 0, x));
        x += w;
    }

    int middleRow = winHeight / 2;
    int trailLength = 5;

    std::vector<int> indices(polyphonicLength);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), context.rng);
    for (int idx : indices) {
        for (int row = 0; row < middleRow + trailLength; row++) {
            int eraseIdx = row - trailLength;
            if (row <= middleRow) {
                mvwaddch(subwins[idx], row, (col_width - 1) / 2,
                         polyphonic[idx]);
            }
            if (eraseIdx >= 0) {
                mvwaddch(subwins[idx], eraseIdx, (col_width - 1) / 2, ' ');
            }
            wrefresh(subwins[idx]);
            std::this_thread::sleep_for(std::chrono::milliseconds(83));
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(3500));

    std::shuffle(indices.begin(), indices.end(), context.rng);

    for (int idx : indices) {
        for (int row = middleRow; row < winHeight + trailLength; row++) {
            int eraseIdx = row - trailLength;
            if (row < winHeight) {
                mvwaddch(subwins[idx], row, (col_width - 1) / 2,
                         polyphonic[idx]);
            }
            if (eraseIdx >= middleRow) {
                mvwaddch(subwins[idx], eraseIdx, (col_width - 1) / 2, ' ');
            }
            wrefresh(subwins[idx]);
            std::this_thread::sleep_for(std::chrono::milliseconds(83));
        }
    }

    for (auto w : subwins) {
        overwrite(w, paddedWindow);
        delwin(w);
    }
    // Copy the final frame to the parent window before deleting paddedWindow
    overwrite(paddedWindow, context.window);
    delwin(paddedWindow);
}
