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
    int waitTime = 10;
    static int phase = 0;

    int substeps = 6;  // More detail per column
    for (int x = 0; x < cols; ++x) {
        for (int sub = 0; sub < substeps; ++sub) {
            double fx = x + (double)sub / substeps;
            double y = amplitude * std::sin(frequency * (fx + phase));
            double fy = mid_row + y;
            int draw_y = static_cast<int>(fy);
            if (draw_y >= 0 && draw_y < rows) {
                // Intensity: how close fy is to the center of the cell
                double dist = std::abs(fy - draw_y);
                double norm = std::min(dist / 0.5, 1.0);  // 0=center, 1=far
                int idx =
                    (int)((1.0 - norm) * (intensityChars.size() - 1) + 0.5);
                idx = std::clamp(idx, 0, (int)intensityChars.size() - 1);
                // Clamp idx manually (C++17 std::clamp may not be available)
                if (idx < 0)
                    idx = 0;
                if (idx >= (int)intensityChars.size())
                    idx = (int)intensityChars.size() - 1;
                mvwaddch(context.window, draw_y, x, intensityChars[idx]);
            }
        }
    }
    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
    phase = (phase + 2) % cols;
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
