/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "verbs.h"

#include <ncurses.h>

#include <algorithm>
#include <thread>

#include "../util/colours.h"

constexpr int CHAR_SLEEP_MS = 40;
constexpr int WORD_SLEEP_MS = 300;

void Verbs::drawFrame(const AnimationContext &context) {
    int rows, cols;
    context.getDimensions(rows, cols);
    int centerY = rows / 2;

    // Shuffle the verbs before displaying
    std::vector<std::string> shuffledVerbs = verbs;
    std::shuffle(shuffledVerbs.begin(), shuffledVerbs.end(), context.rng);

    auto printWord = [&](const std::string &word) {
        // Calculate startX to center this word
        int wordStartX = cols / 2 - static_cast<int>(word.length()) / 2;

        for (size_t i = 0; i < word.length(); ++i) {
            mvwaddch(context.window, centerY, wordStartX + i, word[i]);
            wrefresh(context.window);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(CHAR_SLEEP_MS));
        }
        // TODO: Change this so each word prints in the same time, so divive the
        // time by word length.
        // I guess the time to print and time to sleep could be the same, one
        // beat?
        std::this_thread::sleep_for(std::chrono::milliseconds(WORD_SLEEP_MS));
    };

    for (const auto &word : shuffledVerbs) {
        printWord(word);

        // Clear line here, so the final polyphonic can be faded
        mvwhline(context.window, centerY, 0, ' ', cols);
    }

    // Print final "POLYPHONIC" in a random colour, with color logic outside the
    // loop
    Gradient grad = getRandomGradient(context.rng);
    int colorPair = getColourIndex(grad, 0);

    wattron(context.window, COLOR_PAIR(colorPair));
    printWord(polyphonic);
    wattroff(context.window, COLOR_PAIR(colorPair));

    std::this_thread::sleep_for(std::chrono::milliseconds(WORD_SLEEP_MS * 3));

    printWord("          ");

    wrefresh(context.window);
}