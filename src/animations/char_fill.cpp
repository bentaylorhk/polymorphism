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

    int wordIdx = 0;
    bool outputColours = false;
    int currentColourIdx = 0;
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {

            // Colour letters individually, to account for final truncated words
            // being coloured
            if (outputColours) {
              wattron(context.window, COLOR_PAIR(currentColourIdx));
            }

            mvwaddch(context.window, y, x, context.word[wordIdx]);

            if (outputColours) {
              wattroff(context.window, COLOR_PAIR(currentColourIdx));
            }

            wordIdx++;

            if (wordIdx >= context.wordLen()) {
                wordIdx = 0;

                if (outputColours) {
                    outputColours = false;
                } else {
                    outputColours = dist(context.rng);
                    if (outputColours) {
                        currentColourIdx = getRandomColourIndex(context.rng);
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
