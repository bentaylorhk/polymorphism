/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "fuzz_out.h"

#include <ncurses.h>

#include <algorithm>
#include <thread>
#include <vector>

#include "../util/common.h"

void FuzzOut::drawFrame(const AnimationContext &context) {
    std::vector<std::pair<int, int>> filledCells =
        getFilledCells(context.window);

    std::shuffle(filledCells.begin(), filledCells.end(), context.rng);

    for (const auto &[y, x] : filledCells) {
        mvwaddch(context.window, y, x, ' ');

        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_THIRTY_SECOND_BEAT));
    }
}