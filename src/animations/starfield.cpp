/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "starfield.h"

#include <ncurses.h>

#include <cmath>
#include <random>
#include <thread>
#include <vector>

#include "../util/colours.h"
#include "../util/common.h"

struct Star {
    double x, y, z;
    int colorPair;
};

void Starfield::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    werase(context.window);

    // Starfield parameters
    int numStars = winWidth * winHeight / 32;  // fewer stars for clarity
    double maxDepth = 18.0;
    double minDepth = 2.0;
    double speed = 0.18;
    int frames = 800;
    std::vector<Star> stars(numStars);
    std::mt19937 rng(context.rng());
    // Overshoot factor to ensure stars/trails go past screen edges (for init
    // only)
    double overshoot = 1.2;  // slightly >1 so stars can start off-screen
    std::uniform_real_distribution<double> distX(-overshoot, overshoot);
    std::uniform_real_distribution<double> distY(-overshoot, overshoot);
    std::uniform_real_distribution<double> distZ(minDepth, maxDepth);

    // Use a random gradient for star colors
    Gradient gradient = getRandomGradient(rng);
    std::uniform_int_distribution<int> gradIdxDist(0, GRADIENT_LENGTH - 1);

    // Initialize stars
    for (auto &s : stars) {
        s.x = distX(rng);
        s.y = distY(rng);
        s.z = distZ(rng);
        int gradIdx = gradIdxDist(rng);
        s.colorPair = getColourIndex(gradient, gradIdx);
    }

    // Persistent trail buffer for fading trails
    struct TrailPixel {
        int colorPair = 0;
        double intensity = 0.0;
    };
    std::vector<std::vector<TrailPixel>> trailBuffer(
        winHeight, std::vector<TrailPixel>(winWidth));
    double trailDecay = 0.82;  // how quickly trails fade per frame

    // Add star trails and a more dynamic starfield effect
    int trailLength = 6;
    double trailFade = 0.7;  // how quickly trails fade
    for (int frame = 0; frame < frames; ++frame) {
        // Fade trail buffer
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                trailBuffer[y][x].intensity *= trailDecay;
                if (trailBuffer[y][x].intensity < 0.01)
                    trailBuffer[y][x].intensity = 0.0;
            }
        }
        // Draw star trails (from farthest to closest for proper overlap)
        for (auto &s : stars) {
            for (int t = trailLength; t >= 0; --t) {
                double trail_z = s.z + t * speed * 1.2;
                if (trail_z > maxDepth)
                    continue;
                // Standard perspective projection: map [-1,1] to full screen
                double px = s.x / trail_z;
                double py = s.y / trail_z;
                int sx = (int)(winWidth / 2 + px * winWidth / 2);
                int sy = (int)(winHeight / 2 + py * winHeight / 2);
                // Draw trail even if head is off screen, as long as this
                // segment is on screen
                if (sx >= 0 && sx < winWidth && sy >= 0 && sy < winHeight) {
                    double norm = 1.0 - std::log(trail_z - minDepth + 1) /
                                            std::log(maxDepth - minDepth + 1);
                    norm *= std::pow(trailFade, t);  // fade trail
                    if (norm < 0)
                        norm = 0;
                    if (norm > 1)
                        norm = 1;
                    if (norm > trailBuffer[sy][sx].intensity) {
                        trailBuffer[sy][sx].intensity = norm;
                        trailBuffer[sy][sx].colorPair = s.colorPair;
                    }
                }
            }
        }
        werase(context.window);
        // Draw trail buffer
        for (int y = 0; y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                double norm = trailBuffer[y][x].intensity;
                int idx =
                    (int)(norm * (blankedIntensityChars.size() - 1) + 0.5);
                if (idx < 0)
                    idx = 0;
                if (idx >= (int)blankedIntensityChars.size())
                    idx = (int)blankedIntensityChars.size() - 1;
                char c = blankedIntensityChars[idx];
                if (c != ' ' && norm > 0.01) {
                    wattron(context.window,
                            COLOR_PAIR(trailBuffer[y][x].colorPair));
                    mvwaddch(context.window, y, x, c);
                    wattroff(context.window,
                             COLOR_PAIR(trailBuffer[y][x].colorPair));
                }
            }
        }
        // Draw POLYPHONIC in white, centered
        int text_len = polyphonic.size();
        int text_x = winWidth / 2 - text_len / 2;
        int text_y = winHeight / 2;
        wattron(context.window, COLOR_PAIR(0));
        mvwprintw(context.window, text_y, text_x, "%s", polyphonic.c_str());
        wattroff(context.window, COLOR_PAIR(0));
        // Update star positions and recycle
        for (auto &s : stars) {
            s.z -= speed;
            if (s.z < minDepth) {
                s.x = distX(rng);
                s.y = distY(rng);
                s.z = maxDepth;
                int gradIdx = gradIdxDist(rng);
                s.colorPair = getColourIndex(gradient, gradIdx);
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    werase(context.window);
    wrefresh(context.window);
}