/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <ncurses.h>

#include <sstream>
#include <string>
#include <vector>

constexpr int BPM = 125;

constexpr int MS_PER_BEAT = 60000 / BPM;

constexpr int MS_PER_HALF_BEAT = MS_PER_BEAT / 2;
constexpr int MS_PER_QUARTER_BEAT = MS_PER_BEAT / 4;
constexpr int MS_PER_EIGHTH_BEAT = MS_PER_BEAT / 8;
constexpr int MS_PER_SIXTEENTH_BEAT = MS_PER_BEAT / 16;
constexpr int MS_PER_THIRTY_SECOND_BEAT = MS_PER_BEAT / 32;
constexpr int MS_PER_SIXTY_FOURTH_BEAT = MS_PER_BEAT / 64;

constexpr int MS_PER_DOUBLE_BEAT = MS_PER_BEAT * 2;     // 1 sec
constexpr int MS_PER_TRIPLE_BEAT = MS_PER_BEAT * 3;     // 1.5 sec
constexpr int MS_PER_QUADRUPLE_BEAT = MS_PER_BEAT * 4;  // 2 sec
constexpr int MS_PER_OCTUPLE_BEAT = MS_PER_BEAT * 8;    // 4 sec

class AsciiArt {
   public:
    AsciiArt(const std::vector<std::vector<char>> &input) {
        width = 0;
        for (const auto &row : input) {
            width = std::max(width, (int)row.size());
        }
        height = input.size();

        text.assign(height, std::vector<char>(width, ' '));

        for (int y = 0; y < input.size(); ++y) {
            for (int x = 0; x < input[y].size(); ++x) {
                if (x < input[y].size()) {
                    text[y][x] = input[y][x];
                }
            }
        }
    }

    AsciiArt(const std::string &input) {
        width = 0;
        height = 0;

        std::istringstream stream(input);
        std::string line;
        while (std::getline(stream, line)) {
            width = std::max(width, (int)line.length());
            height++;
        }

        text.assign(height, std::vector<char>(width, ' '));

        int row = 0, col = 0;
        for (char c : input) {
            if (c == '\n') {
                row++;
                col = 0;
                if (row >= height) {
                    break;
                }
            } else {
                text[row][col++] = c;
            }
        }
    }

    char getChar(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            throw std::out_of_range("AsciiArt::getChar: Index out of range");
        }
        return text[y][x];
    }

    int getWidth() const {
        return width;
    }
    int getHeight() const {
        return height;
    }

   private:
    std::vector<std::vector<char>> text;
    int width;
    int height;
};

double easeInOutQuad(double t);

double easeOutQuad(double t);

double easeInQuad(double t);

std::string toLower(const std::string &input);

const std::vector<char> intensityChars = {'.', 'o', 'x', '*',
                                          '#', 'X', '$', '@'};

const std::vector<char> blankedIntensityChars = [] {
    std::vector<char> v = intensityChars;
    v.insert(v.begin(), ' ');
    return v;
}();

std::vector<std::pair<int, int>> getFilledCells(WINDOW *window);

AsciiArt loadAsciiArt(const std::string &path);
