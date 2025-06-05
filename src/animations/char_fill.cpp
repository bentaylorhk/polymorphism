/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "char_fill.h"

#include <ncurses.h>

#include <thread>

#include "../util/colours.h"
#include "../util/common.h"

void CharFill::drawFrame(const AnimationContext &context) {
    wclear(context.window);
    int rows, cols;
    context.getDimensions(rows, cols);

    std::bernoulli_distribution dist(0.1);

    int polyphonicIndex = 0;
    bool outputColours = false;
    int currentColourIndex = 0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            mvwaddch(context.window, y, x, polyphonic[polyphonicIndex]);

            polyphonicIndex++;
            if (polyphonicIndex >= polyphonic.size()) {
                polyphonicIndex = 0;

                if (outputColours) {
                    wattroff(context.window, COLOR_PAIR(currentColourIndex));
                    outputColours = false;
                } else {
                    outputColours = dist(context.rng);
                    if (outputColours) {
                        currentColourIndex = getRandomColourIndex(context.rng);
                        wattron(context.window, COLOR_PAIR(currentColourIndex));
                    }
                }
            }

            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_THIRTY_SECOND_BEAT));
            wrefresh(context.window);
        }
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(MS_PER_QUADRUPLE_BEAT));
}