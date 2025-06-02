/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <string>
#include <vector>

const std::string polyphonic = "POLYPHONIC";

using AsciiArt = std::vector<std::vector<char>>;

void getStringDimensions(const std::string &input, int &width, int &height);

AsciiArt stringTo2DArray(const std::string &input, int width, int height);

double easeInOutQuad(double t);

double easeOutQuad(double t);

double easeInQuad(double t);

std::string toLower(const std::string &input);
