/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "game_of_life.h"

#include <ncurses.h>

#include <chrono>
#include <thread>
#include <vector>

void GameOfLife::drawFrame(const AnimationContext &context) {
    // Load the logo from asciiArt in context
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    int logoHeight = context.asciiArt.size();
    int logoWidth = context.asciiArt[0].size();

    // Center the logo in the window
    int yOffset = ((winHeight - logoHeight) / 2) - 2;
    int xOffset = (winWidth - logoWidth) / 2;

    // Create a grid for the whole window
    std::vector<std::vector<bool>> grid(winHeight,
                                        std::vector<bool>(winWidth, false));

    for (int y = 0; y < logoHeight; ++y) {
        for (int x = 0; x < logoWidth; ++x) {
            // TODO: Might need better handling for logo
            if (context.asciiArt[y][x] == '$') {
                int gy = y + yOffset;
                int gx = x + xOffset;
                if (gy >= 0 && gy < winHeight && gx >= 0 && gx < winWidth)
                    grid[gy][gx] = true;
            }
        }
    }

    auto drawGrid = [&]() {
        werase(context.window);
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                if (grid[y][x]) {
                    mvwaddch(context.window, y, x, '#');
                }
            }
        }
        wrefresh(context.window);
    };

    // Waiting before animation to see logo
    drawGrid();
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));

    int steps = 125;
    for (int step = 0; step < steps; ++step) {
        drawGrid();
        std::this_thread::sleep_for(std::chrono::milliseconds(83));
        std::vector<std::vector<bool>> next = grid;
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                int live = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dy == 0 && dx == 0)
                            continue;
                        int ny = y + dy, nx = x + dx;
                        if (ny >= 0 && ny < winHeight && nx >= 0 &&
                            nx < winWidth && grid[ny][nx])
                            ++live;
                    }
                }
                if (grid[y][x]) {
                    next[y][x] = (live == 2 || live == 3);
                } else {
                    next[y][x] = (live == 3);
                }
            }
        }
        grid = next;
    }
}
