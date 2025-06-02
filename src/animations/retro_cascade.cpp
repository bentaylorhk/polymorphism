/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "retro_cascade.h"

#include <ncurses.h>

#include <algorithm>
#include <chrono>
#include <random>
#include <thread>

void RetroCascade::drawFrame(const AnimationContext &context) {
    int n = polyphonic.size();
    int winHeight, winWidth;
    getmaxyx(context.window, winHeight, winWidth);
    int col_width = winWidth / n;
    int col_rem = winWidth % n;
    std::vector<WINDOW *> subwins;
    int x = 0;
    for (int i = 0; i < n; ++i) {
        int w = col_width + (i < col_rem ? 1 : 0);
        subwins.push_back(derwin(context.window, winHeight, w, 0, x));
        x += w;
    }

    int middleRow = winHeight / 2;
    int trailLength = 5;

    std::vector<int> indices(n);
    std::iota(indices.begin(), indices.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);
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
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
        }
        werase(subwins[idx]);
        mvwaddch(subwins[idx], winHeight / 2, (col_width - 1) / 2,
                 polyphonic[idx]);
        wrefresh(subwins[idx]);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    for (auto w : subwins) {
        delwin(w);
    }
}
