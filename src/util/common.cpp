/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "common.h"

#include <ncurses.h>

#include <algorithm>
#include <fstream>
#include <sstream>

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

AsciiArt loadAsciiArt(const std::string &path) {
    std::string fileContent;
    std::string fullPath = std::string(ASCII_ART_DIR) + path;
    std::ifstream file(fullPath);

    if (file) {
        fileContent.assign((std::istreambuf_iterator<char>(file)),
                           (std::istreambuf_iterator<char>()));
    } else {
        throw std::runtime_error("Failed to open ASCII art file: " + fullPath);
    }

    return AsciiArt(fileContent);
}
