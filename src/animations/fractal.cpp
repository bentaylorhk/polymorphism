/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "fractal.h"

#include <ncurses.h>

#include <cmath>
#include <thread>

#include "../util/common.h"

constexpr int FRACTAL_FRAMES = 400;
constexpr double ZOOM_FACTOR = 0.97;
constexpr double INIT_SCALE = 2;
constexpr int BASE_MAX_ITER = 10;

void Fractal::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    double cx = -0.743643887037158704752191506114774;
    double cy = 0.131825904205311970493132056385139;
    double scale = INIT_SCALE;
    int maxIter = BASE_MAX_ITER;

    for (int frame = 0; frame < FRACTAL_FRAMES; ++frame) {
        // Dynamically adjust maxIter for clarity at each zoom level
        // As we zoom in (scale decreases), increase maxIter linearly
        int dynamicMaxIter = BASE_MAX_ITER + (int)((INIT_SCALE - scale) * 120);
        if (dynamicMaxIter < BASE_MAX_ITER)
            dynamicMaxIter = BASE_MAX_ITER;
        werase(context.window);
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                double re =
                    cx + (x - winWidth / 2.0) * (scale * 2.0) / winWidth;
                double im =
                    cy + (y - winHeight / 2.0) * (scale * 2.0) / winHeight;
                double zx = 0, zy = 0;
                int iter = 0;
                while (zx * zx + zy * zy < 4.0 && iter < dynamicMaxIter) {
                    double tmp = zx * zx - zy * zy + re;
                    zy = 2.0 * zx * zy + im;
                    zx = tmp;
                    iter++;
                }
                double norm = iter + 1 - log2(log2(zx * zx + zy * zy));
                norm = std::clamp(norm / dynamicMaxIter, 0.0, 1.0);
                int idx = (int)(norm * (blankedIntensityChars.size() - 1));
                idx = std::clamp(idx, 0, (int)blankedIntensityChars.size() - 1);
                mvwaddch(context.window, y, x, blankedIntensityChars[idx]);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
        scale *= ZOOM_FACTOR;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    werase(context.window);
    wrefresh(context.window);
}
