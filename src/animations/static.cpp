/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "static.h"

#include <ncurses.h>

#include <thread>

void Static::drawFrame(const AnimationContext &context) {
    int rows, cols;
    context.getDimensions(rows, cols);

    // Duration of static in ms
    int numFrames = 1000;

    // Use blankedIntensityChars for static
    std::uniform_int_distribution<int> charDist(
        0, blankedIntensityChars.size() - 1);

    // Show static noise by updating 20 random cells per frame, with fade in/out
    std::vector<std::pair<int, int>> allCells;
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            allCells.emplace_back(y, x);

    for (int frame = 0; frame < numFrames; ++frame) {
        // Calculate fade progress (0.0 to 1.0)
        double progress = static_cast<double>(frame) / (numFrames - 1);
        float ease = 1.0f;
        if (progress < 0.15f) {
            ease = easeInOutQuad(progress / 0.15f);  // Fade in
        } else if (progress > 0.9f) {
            ease = 0.0f;  // Last 10%: no static
        } else if (progress > 0.8f) {
            ease = 1.0f - easeInOutQuad((progress - 0.8f) / 0.1f);  // Fade out
        }

        // Map fade to max char index: at fade=0, only blank; at fade=1, all
        // chars
        int maxCharIdx =
            static_cast<int>(ease * (blankedIntensityChars.size() - 1));
        if (maxCharIdx < 0)
            maxCharIdx = 0;
        if (maxCharIdx >= (int)blankedIntensityChars.size())
            maxCharIdx = blankedIntensityChars.size() - 1;
        std::uniform_int_distribution<int> fadeCharDist(0, maxCharIdx);

        std::shuffle(allCells.begin(), allCells.end(), context.rng);
        for (int i = 0; i < 50 && i < (int)allCells.size(); ++i) {
            auto [y, x] = allCells[i];
            char c = blankedIntensityChars[fadeCharDist(context.rng)];
            mvwaddch(context.window, y, x, c);
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTY_FOURTH_BEAT));
    }

    std::vector<std::pair<int, int>> filledCells =
        getFilledCells(context.window);

    std::shuffle(filledCells.begin(), filledCells.end(), context.rng);

    for (const auto &[y, x] : filledCells) {
        mvwaddch(context.window, y, x, ' ');

        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }

    wrefresh(context.window);
}