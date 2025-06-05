/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "diagonal_fill_wipe.h"

#include <ncurses.h>

#include <thread>

void DiagonalFillWipe::drawFrame(const AnimationContext &context) {
    int rows, cols;
    context.getDimensions(rows, cols);

    // Fill the window diagonally (top-left to bottom-right)
    int max_diagonal = rows + cols - 2;
    for (int d = 0; d <= max_diagonal; ++d) {
        for (int y = 0; y < rows; ++y) {
            int x = d - y;
            if (x >= 0 && x < cols) {
                mvwaddch(context.window, y, x,
                         polyphonic[d % polyphonic.size()]);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTY_FOURTH_BEAT));
    }

    // Wipe the window diagonally in the same way
    for (int d = 0; d <= max_diagonal; ++d) {
        for (int y = 0; y < rows; ++y) {
            int x = d - y;
            if (x >= 0 && x < cols) {
                mvwaddch(context.window, y, x, ' ');
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTY_FOURTH_BEAT));
    }
}