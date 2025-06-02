/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <string>
#include <vector>

const std::string polyphonic = "POLYPHONIC";

constexpr int BPM = 125;
const int WAIT_TIME = 60000 / BPM;  // Milliseconds per beat

using AsciiArt = std::vector<std::vector<char>>;

void getStringDimensions(const std::string &input, int &width, int &height);

AsciiArt stringTo2DArray(const std::string &input, int width, int height);

double easeInOutQuad(double t);

double easeOutQuad(double t);

double easeInQuad(double t);

std::string toLower(const std::string &input);
