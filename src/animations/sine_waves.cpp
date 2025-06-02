#include "sine_waves.h"

#include <ncurses.h>

#include <algorithm>
#include <cmath>
#include <thread>

#include "../common.h"

void SineWaves::drawFrame(const AnimationContext &context) {
    werase(context.window);

    int rows, cols;
    context.getDimensions(rows, cols);

    int mid_row = rows / 2;
    double amplitude = rows * 0.4;               // 80% of screen height
    double frequency = 2.0 * M_PI / cols * 2.0;  // 2 full waves across screen
    static int phase = 0;

    int substeps = 6;       // More detail per column
    int frames = cols * 2;  // Number of frames for a full cycle
    for (int frame = 0; frame < frames; ++frame) {
        werase(context.window);
        for (int x = 0; x < cols; ++x) {
            // Track the maximum intensity index for this cell
            int max_idx[rows];
            for (int i = 0; i < rows; ++i)
                max_idx[i] = -1;
            for (int sub = 0; sub < substeps; ++sub) {
                double fx = x + (double)sub / substeps;
                double y = amplitude * std::sin(frequency * (fx + phase));
                double fy = mid_row + y;
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
        std::this_thread::sleep_for(std::chrono::milliseconds(83));
        phase = (phase + 2) % cols;
    }
    // Hold final frame for a moment
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
