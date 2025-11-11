/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "scrolling_polyphonic.h"

#include <ncurses.h>

#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../util/colours.h"
#include "../util/common.h"

void ScrollingPolyphonic::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    AsciiArt polyphonic = loadAsciiArt("polyphonic.txt");
    int artHeight = polyphonic.getHeight();
    int artWidth = polyphonic.getWidth();
    int vertOffset = (winHeight - artHeight) / 2;

    AsciiArt renae = loadAsciiArt("renae/padded-logo.txt");
    int renaeHeight = renae.getHeight();
    int renaeWidth = renae.getWidth();
    int renaeY = (winHeight - renaeHeight) / 2;
    int renaeX = (winWidth - renaeWidth) / 2;

    // Ripple reveal animation for renae logo
    int maxRadius = std::max(renaeWidth, renaeHeight);
    for (int radius = 0; radius <= maxRadius; ++radius) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
        werase(context.window);
        for (int y = 0; y < renaeHeight; ++y) {
            for (int x = 0; x < renaeWidth; ++x) {
                int dx = x - renaeWidth / 2;
                int dy = y - renaeHeight / 2;
                int dist = static_cast<int>(std::sqrt(dx * dx + dy * dy));
                if (dist <= radius) {
                    char c = renae.getChar(x, y);
                    if (c == '*') {
                        mvwaddch(context.window, renaeY + y, renaeX + x, ' ');
                    } else if (c != ' ') {
                        mvwaddch(context.window, renaeY + y, renaeX + x, c);
                    }
                }
            }
        }
        wrefresh(context.window);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_BEAT));

    // TODO: better colour once chose, probably main colours.
    int colour = getRandomColourIndex(context.rng);

    int repetitions = 12;
    int gap = 2;
    int totalArtWidth = repetitions * (artWidth + gap);
    int totalFrames = winWidth + totalArtWidth;
    // Easing parameters
    double easeLen = 0.4;  // length of the ease in/out
    for (int frame = -winWidth; frame < totalArtWidth; ++frame) {
        werase(context.window);
        wattron(context.window, COLOR_PAIR(colour));
        for (int y = 0; y < artHeight && vertOffset + y < winHeight; ++y) {
            for (int x = 0; x < winWidth; ++x) {
                int artPos = frame + x;
                if (artPos < 0 || artPos >= totalArtWidth)
                    continue;
                int rep = artPos / (artWidth + gap);
                int offset = artPos % (artWidth + gap);
                if (offset < artWidth) {
                    char c = polyphonic.getChar(offset, y);
                    if (c != ' ') {
                        mvwaddch(context.window, vertOffset + y, x, c);
                    }
                }  // else: gap, leave blank
            }
        }
        wattroff(context.window, COLOR_PAIR(colour));
        // Easing for wave amplitude, no flat region
        double easeNormalized = 1.0;
        int animFrame = frame + winWidth;  // 0 to totalFrames-1
        double t = (double)animFrame / (double)totalFrames;
        if (t < 0.25) {
            easeNormalized = easeInOutQuad(t / 0.25);  // 0 to 1
        } else if (t < 0.5) {
            easeNormalized = easeInOutQuad((0.5 - t) / 0.25);  // 1 to 0
        } else if (t < 0.75) {
            easeNormalized = easeInOutQuad((t - 0.5) / 0.25);  // 0 to 1
        } else {
            easeNormalized = easeInOutQuad((1.0 - t) / 0.25);  // 1 to 0
        }
        // Ensure minimum amplitude to avoid jump start due to low terminal
        // resolution
        double minEase = 0.28;  // floor for ease
        double ease = minEase + (1.0 - minEase) * easeNormalized;

        for (int y = 0; y < renaeHeight; ++y) {
            for (int x = 0; x < renaeWidth; ++x) {
                char c = renae.getChar(x, y);
                // TODO: 2 or 3 or else?
                int waveY =
                    renaeY +
                    static_cast<int>(sin(frame * 0.15 + x * 0.3) * 3.0 * ease);
                if (c == '*') {
                    mvwaddch(context.window, waveY + y, renaeX + x, ' ');
                } else if (c != ' ') {
                    mvwaddch(context.window, waveY + y, renaeX + x, c);
                }
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }
}
