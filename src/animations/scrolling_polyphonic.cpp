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

    // Pre-calculate constants for performance
    const double frameToTotalFrames = 1.0 / totalFrames;
    const double renaeWidthInv = 1.0 / (renaeWidth - 1);
    const double sweepWidth = 0.1;
    const double sweepWidthInv = 1.0 / (1.0 - sweepWidth);
    const double minEase = 0.28;
    const double easeRange = 1.0 - minEase;
    const int artWidthPlusGap = artWidth + gap;

    for (int frame = -winWidth; frame < totalArtWidth; ++frame) {
        werase(context.window);

        // Render scrolling polyphonic text
        wattron(context.window, COLOR_PAIR(colour));
        const int maxY = std::min(artHeight, winHeight - vertOffset);
        for (int y = 0; y < maxY; ++y) {
            const int screenY = vertOffset + y;
            for (int x = 0; x < winWidth; ++x) {
                const int artPos = frame + x;
                if (artPos >= 0 && artPos < totalArtWidth) {
                    const int offset = artPos % artWidthPlusGap;
                    if (offset < artWidth) {
                        const char c = polyphonic.getChar(offset, y);
                        if (c != ' ') {
                            mvwaddch(context.window, screenY, x, c);
                        }
                    }
                }
            }
        }
        wattroff(context.window, COLOR_PAIR(colour));

        // Pre-calculate frame-based values for renae animation
        const int animFrame = frame + winWidth;
        const double globalT = animFrame * frameToTotalFrames;
        const double frameWave = frame * 0.15;

        // Render animated renae logo
        for (int y = 0; y < renaeHeight; ++y) {
            const int baseScreenY = renaeY + y;
            for (int x = 0; x < renaeWidth; ++x) {
                const char c = renae.getChar(x, y);
                if (c == ' ')
                    continue;  // Skip empty characters early

                // Calculate easing with pre-computed values
                const double xNormalized = (renaeWidth - 1 - x) * renaeWidthInv;
                double localT =
                    (globalT - xNormalized * sweepWidth) * sweepWidthInv;
                localT = std::max(0.0, std::min(1.0, localT));

                double easeNormalized;
                if (localT < 0.25) {
                    easeNormalized = easeInOutQuad(localT * 4.0);
                } else if (localT < 0.5) {
                    easeNormalized = easeInOutQuad((0.5 - localT) * 4.0);
                } else if (localT < 0.75) {
                    easeNormalized = easeInOutQuad((localT - 0.5) * 4.0);
                } else {
                    easeNormalized = easeInOutQuad((1.0 - localT) * 4.0);
                }

                const double ease = minEase + easeRange * easeNormalized;
                const int waveY =
                    baseScreenY +
                    static_cast<int>(sin(frameWave + x * 0.3) * 3.0 * ease);

                if (c == '*') {
                    mvwaddch(context.window, waveY, renaeX + x, ' ');
                } else {
                    mvwaddch(context.window, waveY, renaeX + x, c);
                }
            }
        }

        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));
}
