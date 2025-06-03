/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "hypnotic_waves.h"

#include <ncurses.h>

#include <cmath>
#include <string>
#include <thread>
#include <vector>

#include "../common.h"

void HypnoticWaves::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    int frames = 600;
    float t = 0.0f;
    float dt = 0.09f;
    float freq1 = 0.13f, freq2 = 0.09f, freq3 = 0.07f;
    float amp1 = 1.7f, amp2 = 1.2f, amp3 = 0.8f;

    for (int frame = 0; frame < frames; ++frame, t += dt) {
        werase(context.window);
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                // Normalized coordinates
                float nx = (2.0f * x / winWidth - 1.0f) * 3.0f;
                float ny = (2.0f * y / winHeight - 1.0f) * 1.5f;
                // Compose several sine/cosine waves
                float v = 0.0f;
                v += std::sin(nx * freq1 + t) * amp1;
                v += std::cos(ny * freq2 - t * 1.2f) * amp2;
                v += std::sin((nx + ny) * freq3 + t * 0.7f) * amp3;
                // Radial ripple
                float r = std::sqrt(nx * nx + ny * ny);
                v += std::sin(r * 2.5f - t * 1.5f) * 1.1f;
                // Normalize to [0, 1]
                float norm = (std::tanh(v) + 1.0f) / 2.0f;
                int idx =
                    static_cast<int>(norm * (blankedIntensityChars.size() - 1));
                mvwaddch(context.window, y, x, blankedIntensityChars[idx]);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(35));
    }
}
