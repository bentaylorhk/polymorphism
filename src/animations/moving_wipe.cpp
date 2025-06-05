/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "moving_wipe.h"

#include <ncurses.h>

#include <thread>
#include <vector>

void MovingWipe::drawFrame(const AnimationContext &context) {
    int rows, cols;
    context.getDimensions(rows, cols);

    // Build movingChars: a-z, A-Z, 0-9, then some symbols
    std::vector<char> movingChars;
    for (char c = 'a'; c <= 'z'; ++c)
        movingChars.push_back(c);
    for (char c = 'A'; c <= 'Z'; ++c)
        movingChars.push_back(c);
    for (char c = '0'; c <= '9'; ++c)
        movingChars.push_back(c);
    const std::vector<char> extraChars = {'!', '@', '#', '$', '%',
                                          '^', '&', '*', '-', '+',
                                          '=', '?', '/', '|', '~'};
    movingChars.insert(movingChars.end(), extraChars.begin(), extraChars.end());
    int numChars = movingChars.size();

    // The wave will move to the right, text persists and shifts as a triangle
    // Build the full sequence: movingChars, last char 5x, reversed movingChars,
    // whitespace
    std::vector<char> fullSeq = movingChars;
    for (int i = 0; i < 5; ++i)
        fullSeq.push_back(movingChars.back());
    for (auto it = movingChars.rbegin(); it != movingChars.rend(); ++it)
        fullSeq.push_back(*it);
    fullSeq.push_back(' ');
    int seqLen = fullSeq.size();
    // Ensure the wipe continues until the last row is fully cleared
    int maxShift = cols + seqLen + 2 * (rows - 1) - 1;

    // Randomly choose a corner: 0=TL, 1=TR, 2=BL, 3=BR
    std::uniform_int_distribution<int> cornerDist(0, 3);
    int corner = cornerDist(context.rng);

    // Lambda to map (y, x) based on corner
    auto mapYX = [&](int y, int x) -> std::pair<int, int> {
        switch (corner) {
            case 0:  // Top-left
                return {y, x};
            case 1:  // Top-right
                return {y, cols - 1 - x};
            case 2:  // Bottom-left
                return {rows - 1 - y, x};
            case 3:  // Bottom-right
                return {rows - 1 - y, cols - 1 - x};
        }
        return {y, x};
    };

    // Extend the main loop to include the trailing blank wipe
    for (int shift = 0; shift <= maxShift + seqLen; ++shift) {
        int tail = (shift > maxShift) ? (shift - maxShift) : 0;
        for (int y = 0; y < rows; ++y) {
            int edgeX = shift - 2 * y;
            if (edgeX < 0)
                continue;  // Triangle not yet reached this row
            for (int x = 0; x < cols; ++x) {
                if (x > edgeX)
                    continue;  // Outside the triangle
                int charIdx = edgeX - x + shift;
                charIdx = charIdx % seqLen;  // Wrap around
                auto [mappedY, mappedX] = mapYX(y, x);
                if (tail > 0 && charIdx >= seqLen - tail)
                    mvwaddch(context.window, mappedY, mappedX, ' ');
                else
                    mvwaddch(context.window, mappedY, mappedX,
                             fullSeq[charIdx]);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_THIRTY_SECOND_BEAT));
    }
}
