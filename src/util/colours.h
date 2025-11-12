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

/*
polymorphism 1
const std::vector<std::array<int, GRADIENT_LENGTH>> gradients = {
    {207, 206, 205, 204, 203},  // Pink
    {114, 151, 150, 186, 194},  // Cyan
    {214, 220, 221, 222, 223},  // Yellow
    {160, 166, 172, 215, 180},  // Red
    {118, 113, 65, 107, 187},   // Green
};
*/

const std::vector<std::array<int, GRADIENT_LENGTH>> gradients = {
    {207, 206, 205, 204, 203},  // Deep Pink
    {132, 168, 181, 216, 224},  // Pastel Pink
    {52, 88, 124, 131, 108},    // Bronze
    {160, 166, 172, 215, 180},  // Red Orange
    {52, 124, 202, 208, 221},   // Dark Red
};

const int GREY = 238;

void setupColours();

int getColourIndex(Gradient gradient, int index);

int getInverseColourIndex(Gradient gradient, int index);

int getGreyColourIndex();

Gradient getRandomGradient(std::mt19937 &rng);

std::vector<Gradient> getAllRandomGradients(std::mt19937 &rng);

std::vector<Gradient> getNUniqueGradients(std::mt19937 &rng, int n);

int getRandomColourIndex(std::mt19937 &rng);
