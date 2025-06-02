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
    Green,
    Yellow,
    Red,
    Brown,
};

const std::vector<std::array<int, GRADIENT_LENGTH>> gradients = {
    {207, 206, 205, 204, 203},  // Pink
    {46, 40, 34, 28, 22},       // Green
    {214, 220, 221, 222, 223},  // Yellow
    {196, 160, 124, 88, 52},    // Red
    {94, 101, 144, 187, 230},   // Brown
};

void setupColours();

int getColourIndex(Gradient gradient, int index);

Gradient getRandomGradient(std::mt19937& rng);

std::vector<Gradient> getNUniqueGradients(std::mt19937& rng, int n);

int getRandomColourIndex(std::mt19937& rng);