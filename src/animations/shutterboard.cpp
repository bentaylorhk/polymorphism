/*
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "shutterboard.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <thread>

void Shutterboard::run(const AnimationContext &context) {
    int artHeight = context.asciiArt.size();
    // TODO: return an error if art height 0
    int artWidth = context.asciiArt[0].size();

    // Getting a vector of 2D indices
    std::vector<std::pair<int, int>> indices;
    for (int y = 0; y < artHeight; y++) {
        for (int x = 0; x < artWidth; x++) {
            indices.emplace_back(y, x);
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(indices.begin(), indices.end(), g);

    int minDelay = 1;
    int maxDelay = 60;

    int step = 0;
    for (const auto &[y, x] : indices) {
        wrefresh(context.window);

        mvwaddch(context.window, y, x, context.asciiArt[y][x]);

        // Normalising the progress
        double progress = static_cast<double>(step) / (indices.size() - 1);
        double easedFactor = easeOutQuad(progress);

        int delay =
            minDelay + static_cast<int>((maxDelay - minDelay) * easedFactor);

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    wrefresh(context.window);
}