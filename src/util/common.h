/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <ncurses.h>

#include <string>
#include <vector>

const std::string polyphonic = "POLYPHONIC";

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

using AsciiArt = std::vector<std::vector<char>>;

void getStringDimensions(const std::string &input, int &width, int &height);

AsciiArt stringTo2DArray(const std::string &input, int width, int height);

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