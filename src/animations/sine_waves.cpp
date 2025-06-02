/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "sine_waves.h"

#include <ncurses.h>

#include <algorithm>
#include <cmath>
#include <thread>

#include "../common.h"

constexpr int LINE_WAIT_TIME = 25;
constexpr int WAVE_WAIT_TIME = 50;

void SineWaves::drawFrame(const AnimationContext &context) {
    werase(context.window);

    int rows, cols;
    context.getDimensions(rows, cols);

    int mid_row = rows / 2;
    double amplitude = rows * 0.4;               // 80% of screen height
    double frequency = 2.0 * M_PI / cols * 2.0;  // 2 full waves across screen
    static int phase = 0;

    int substeps = 6;   // More detail per column
    int frames = 1000;  // Number of frames for a full cycle

    // Add randomness to sine wave parameters
    double freq1_rand = 1.0 + (context.rng() % 15) * 0.01;      // 1.00 to 1.14
    double freq2_rand = 1.07 + (context.rng() % 15) * 0.01;     // 1.07 to 1.21
    double phase1_rand = (context.rng() % 360) * M_PI / 180.0;  // 0 to 2pi
    double phase2_rand = (context.rng() % 360) * M_PI / 180.0;  // 0 to 2pi
    double amp2_rand = 0.6 + (context.rng() % 20) * 0.02;       // 0.6 to 1.0

    // Intro: animate '@' moving right to left across mid_row
    for (int x = cols - 1; x >= 0; --x) {
        mvwaddch(context.window, mid_row, x, '.');
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(LINE_WAIT_TIME));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Center the wave on the middle of the mid_row cell
    double mid_y = mid_row + 0.5;

    // Sine wave animation
    for (int frame = 0; frame < frames; ++frame) {
        // Calculate ease value for amplitude envelope
        double ease = 1.0;
        if (frame < frames * 0.1) {
            ease = easeInOutQuad((double)frame / (frames * 0.1));
        } else if (frame > frames * 0.9) {
            ease = easeInOutQuad((double)(frames - frame) / (frames * 0.1));
        }
        werase(context.window);
        for (int x = 0; x < cols; ++x) {
            // Track the maximum intensity index for this cell
            int max_idx[rows];
            for (int i = 0; i < rows; ++i)
                max_idx[i] = -1;
            // Calculate the maximum possible amplitude for the sum of the two
            // waves The max sum is amplitude + amplitude * 0.7 = amplitude
            // * 1.7
            double combined_amplitude =
                (rows * 0.4 * ease) /
                (1.0 + amp2_rand);  // scale so max sum fits screen
            double amplitude1 = combined_amplitude;
            double amplitude2 = combined_amplitude * amp2_rand;
            for (int sub = 0; sub < substeps; ++sub) {
                double fx = x + (double)sub / substeps;
                // First sine wave with randomness
                double y1 = amplitude1 *
                            std::sin(frequency * freq1_rand * (fx + phase) +
                                     phase1_rand);
                // Second sine wave: slightly different frequency and phase
                // offset, with randomness
                double y2 = amplitude2 * std::sin(frequency * freq2_rand *
                                                      (fx + phase * 1.13 + 17) +
                                                  phase2_rand);
                // Combine the two
                double y = y1 + y2;
                double fy = mid_y + y;
                int draw_y = static_cast<int>(fy);
                if (draw_y >= 0 && draw_y < rows) {
                    double dist = std::abs(fy - draw_y);
                    double norm = std::min(dist / 0.5, 1.0);  // 0=center, 1=far
                    int idx =
                        (int)((1.0 - norm) * (intensityChars.size() - 1) + 0.5);
                    if (idx < 0)
                        idx = 0;
                    if (idx >= (int)intensityChars.size())
                        idx = (int)intensityChars.size() - 1;
                    if (idx > max_idx[draw_y])
                        max_idx[draw_y] = idx;
                }
            }
            for (int y = 0; y < rows; ++y) {
                if (max_idx[y] >= 0) {
                    mvwaddch(context.window, y, x, intensityChars[max_idx[y]]);
                }
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(WAVE_WAIT_TIME));
        phase = phase + 1;
    }

    // Instantly draw the '@' character across the middle row
    werase(context.window);
    for (int x = 0; x < cols; ++x) {
        mvwaddch(context.window, mid_row, x, '.');
    }
    wrefresh(context.window);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (int x = cols - 1; x >= 0; --x) {
        mvwaddch(context.window, mid_row, x, ' ');
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(LINE_WAIT_TIME));
    }
}
