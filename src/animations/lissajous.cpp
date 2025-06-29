/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "lissajous.h"

#include <ncurses.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>
#include <thread>

#include "../util/colours.h"
#include "../util/common.h"

void Lissajous::drawFrame(const AnimationContext& context) {
    wclear(context.window);

    std::uniform_int_distribution<int> freq_dist(1, 6);

    int cols, rows;
    context.getDimensions(rows, cols);

    // Center and radii
    double cx = cols / 2.0;
    double cy = (rows / 2.0) - 0.5;
    double rx = (cols - 2) / 2.0 * 0.9;  // Leave margin, scale for aspect
    double ry =
        (rows - 2) / 2.0 * 0.9;  // Height scaling for typical terminal aspect

    // Number of steps for smoothness (higher = smoother)
    int steps = 1000;

    Gradient gradient = getRandomGradient(context.rng);
    std::vector<Gradient> gradients = getNUniqueGradients(context.rng, 2);

    // For unique, aesthetic Lissajous curves, use coprime (a, b) pairs and
    // avoid duplicate ratios
    int minFreq = 2, maxFreq = 6;
    std::vector<std::pair<int, int>> possiblePairs;
    for (int a = minFreq; a <= maxFreq; ++a) {
        for (int b = minFreq; b <= maxFreq; ++b) {
            int g = std::gcd(a, b);
            if (g == 1) {
                possiblePairs.emplace_back(a, b);
            }
        }
    }
    // Shuffle for randomness
    std::shuffle(possiblePairs.begin(), possiblePairs.end(), context.rng);
    // Only draw up to 3 unique curves
    int numCurves = std::min(3, (int)possiblePairs.size());
    for (int curveCount = 0; curveCount < numCurves; curveCount++) {
        int a = possiblePairs[curveCount].first;
        int b = possiblePairs[curveCount].second;

        // Calculate fundamental period for early stopping
        int g = std::gcd(a, b);
        double period = 2 * M_PI / g;
        int curveSteps =
            std::ceil(period / (2 * M_PI) * steps);  // Steps for just one cycle
        double dt = period / curveSteps;

        // Progress from white, light gradient colour, strongest gradient
        // colour.
        int colourIndex;
        switch (curveCount) {
            case 0:
                colourIndex = 0;
                break;
            case 1:
                colourIndex = getColourIndex(gradients[0], 0);
                break;
            default:
                colourIndex = getColourIndex(gradients[1], 0);
        }

        wattron(context.window, COLOR_PAIR(colourIndex));

        for (int i = 0; i <= curveSteps; i++) {
            double t = i * dt;
            double x_real = cx + rx * std::sin(a * t);
            double y_real = cy - ry * std::cos(b * t);

            int px = (int)std::round(x_real);
            int py = (int)std::round(y_real);

            // Compute the distance to the centre of the pixel
            double dx = x_real - px;
            double dy = y_real - py;
            double dist =
                std::sqrt(dx * dx + dy * dy);  // 0 (center) to ~0.707 (edge)

            // Map distance to intensity (smaller dist = bolder char)
            // Clamp and invert so 0 is boldest, 0.707 is faintest
            double norm = std::min(dist / 0.7, 1.0);  // 0 to 1, thinner lines
            int idx = (int)((1.0 - norm) * (intensityChars.size() - 1) + 0.5);
            idx = std::clamp(idx, 0, (int)intensityChars.size() - 1);

            mvwaddch(context.window, py, px, intensityChars[idx]);

            wrefresh(context.window);
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_SIXTY_FOURTH_BEAT));
        }

        wattroff(context.window, COLOR_PAIR(colourIndex));
    }

    // Hold at the end for a moment so the curve is visible
    wrefresh(context.window);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MS_PER_QUADRUPLE_BEAT));
}