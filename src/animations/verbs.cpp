/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "verbs.h"

#include <ncurses.h>

#include <algorithm>
#include <thread>

#include "../util/colours.h"

void Verbs::drawFrame(const AnimationContext &context) {
    int rows, cols;
    context.getDimensions(rows, cols);
    int centerY = rows / 2;

    std::vector<std::string> shuffledVerbs = verbs;
    std::shuffle(shuffledVerbs.begin(), shuffledVerbs.end(), context.rng);

    auto printWord = [&](const std::string &word) {
        // Speed to print each character, dependant on length of the word
        int charSleepMs = MS_PER_BEAT / word.length();

        // Calculate startX to center this word, 1 accounting for cursor
        int wordStartX = (cols / 2) - (static_cast<int>(word.length()) / 2) - 1;

        for (size_t i = 0; i < word.length(); ++i) {
            mvwaddch(context.window, centerY, wordStartX + i, word[i]);
            wrefresh(context.window);
            std::this_thread::sleep_for(std::chrono::milliseconds(charSleepMs));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_BEAT));
    };

    curs_set(TRUE);
    for (const auto &word : shuffledVerbs) {
        printWord(word);

        // Clear line here, so the final polyphonic can be faded
        mvwhline(context.window, centerY, 0, ' ', cols);
    }
    curs_set(FALSE);

    // Print final word in a random colour
    Gradient grad = getRandomGradient(context.rng);
    int colorPair = getColourIndex(grad, 0);

    wattron(context.window, COLOR_PAIR(colorPair));
    printWord(context.word);
    wattroff(context.window, COLOR_PAIR(colorPair));


    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));

    // Fading the final word, matching its length to suit timing
    std::string spaces(context.wordLen(), ' ');
    printWord(spaces);

    wrefresh(context.window);
}
