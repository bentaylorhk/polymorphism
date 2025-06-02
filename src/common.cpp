/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "common.h"

#include <ncurses.h>

#include <algorithm>
#include <sstream>

void getStringDimensions(const std::string &input, int &width, int &height) {
    // Initialising the return values
    width = 0;
    height = 0;

    std::istringstream stream(input);
    std::string line;
    while (std::getline(stream, line)) {
        width = std::max(width, (int)line.length());
        height++;
    }
}

AsciiArt stringTo2DArray(const std::string &input, int width, int height) {
    AsciiArt art;
    art.assign(height, std::vector<char>(width, ' '));

    int row = 0, col = 0;
    for (char c : input) {
        if (c == '\n') {
            row++;
            col = 0;
            if (row >= height) {
                break;
            }
        } else {
            art[row][col++] = c;
        }
    }

    return art;
}

double easeInOutQuad(double t) {
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

double easeOutQuad(double t) {
    return t * (2 - t);
}

double easeInQuad(double t) {
    return t * t;
}

std::string toLower(const std::string &input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) {
                       return std::tolower(c);
                   });
    return result;
}

std::vector<std::pair<int, int>> getFilledCells(WINDOW *window) {
    int rows, cols;
    getmaxyx(window, rows, cols);

    std::vector<std::pair<int, int>> filledCells;

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            chtype ch = mvwinch(window, y, x);
            if ((ch & A_CHARTEXT) != ' ') {
                filledCells.emplace_back(y, x);
            }
        }
    }

    return filledCells;
}