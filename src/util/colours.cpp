/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "colours.h"

#include <ncurses.h>

#include <random>

void setupColours() {
    start_color();
    if (!has_colors()) {
        return;
    }

    use_default_colors();

    for (int i = 0; i < gradients.size(); i++) {
        for (int j = 0; j < GRADIENT_LENGTH; j++) {
            init_pair((i * GRADIENT_LENGTH) + j + 1, gradients[i][j], -1);
        }
    }
}

int getColourIndex(Gradient gradient, int index) {
    if (gradient < 0 || gradient >= gradients.size() || index < 0 ||
        index >= GRADIENT_LENGTH) {
        throw std::out_of_range(
            "Invalid gradient or index: gradient=" + std::to_string(gradient) +
            ", index=" + std::to_string(index));
    }
    return (gradient * GRADIENT_LENGTH) + index + 1;
}

Gradient getRandomGradient(std::mt19937& rng) {
    std::uniform_int_distribution<int> dist(0, gradients.size() - 1);
    return static_cast<Gradient>(dist(rng));
}