/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <array>
#include <random>
#include <stdexcept>
#include <vector>

constexpr int GRADIENT_LENGTH = 5;

enum Gradient {
    Pink,
    Cyan,
    Yellow,
    Red,
    Green,
};

const std::vector<std::array<int, GRADIENT_LENGTH>> gradients = {
    {207, 206, 205, 204, 203},  // Pink
    {114, 151, 150, 186, 194},  // Cyan
    {214, 220, 221, 222, 223},  // Yellow
    {160, 166, 172, 215, 180},  // Red
    {118, 113, 65, 107, 187},   // Green
};

void setupColours();

int getColourIndex(Gradient gradient, int index);

int getInverseColourIndex(Gradient gradient, int index);

Gradient getRandomGradient(std::mt19937& rng);

std::vector<Gradient> getAllRandomGradients(std::mt19937& rng);

std::vector<Gradient> getNUniqueGradients(std::mt19937& rng, int n);

int getRandomColourIndex(std::mt19937& rng);