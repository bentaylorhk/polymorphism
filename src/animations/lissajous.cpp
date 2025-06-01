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

void Lissajous::run(const AnimationContext& context) {
    wclear(context.window);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> freq_dist(1, 6);

    std::shuffle(chars.begin(), chars.end(), gen);

    int cols, rows;
    context.getDimensions(cols, rows);

    // Center and radii
    double cx = cols / 2.0;
    double cy = rows / 2.0;
    double rx = (cols - 2) / 2.0 * 0.85;  // Leave margin, scale for aspect
    double ry =
        (rows - 2) / 2.0 * 0.85;  // Height scaling for typical terminal aspect

    // Number of steps for smoothness (higher = smoother)
    int steps = 1000;

    for (int curveCount = 0; curveCount < 3; curveCount++) {
        // Random frequencies for each curve
        int a = freq_dist(gen);
        int b = freq_dist(gen);

        // Calculate fundamental period for early stopping
        int g = std::gcd(a, b);
        double period = 2 * M_PI / g;
        int curveSteps =
            std::ceil(period / (2 * M_PI) * steps);  // Steps for just one cycle
        double dt = period / curveSteps;

        wattron(context.window, COLOR_PAIR(curveCount));

        for (int i = 0; i <= curveSteps; i++) {
            double t = i * dt;
            double x = cx + rx * std::sin(a * t);
            double y = cy - ry * std::cos(b * t);
            mvwaddch(context.window, (int)std::round(y), (int)std::round(x),
                     chars[curveCount]);

            wrefresh(context.window);
            std::this_thread::sleep_for(std::chrono::milliseconds(7));
        }

        wattroff(context.window, COLOR_PAIR(curveCount));
    }

    // Hold at the end for a moment so the curve is visible
    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
}