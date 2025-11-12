/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "wavy_dancers.h"

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

void WavyDancers::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    std::string text = "POLYPHONIC x RENAE x ";
    int textLength = text.length();
    int grey = getGreyColourIndex();

    // Create offset for each alternating line with diagonal pattern
    std::vector<int> lineOffsets;
    for (int y = 0; y < winHeight; y += 2) {
        lineOffsets.push_back(y);  // Create diagonal/jagged start
    }

    // Load dancers
    AsciiArt dancer1 = loadAsciiArt("renae/dancer1.txt");
    AsciiArt dancer2 = loadAsciiArt("renae/dancer2.txt");
    AsciiArt dancer3 = loadAsciiArt("renae/dancer3.txt");
    std::vector<AsciiArt> dancers = {dancer1, dancer2, dancer3};

    // Fade-in animation for text lines
    int fadeInFrames = winWidth;
    int fadeDrawTime = MS_PER_BEAT / fadeInFrames;
    for (int fadeFrame = 0; fadeFrame <= fadeInFrames; ++fadeFrame) {
        werase(context.window);

        wattron(context.window, COLOR_PAIR(grey));
        for (int y = 0, lineIndex = 0; y < winHeight; y += 2, ++lineIndex) {
            if (lineIndex >= lineOffsets.size())
                break;

            int offset = lineOffsets[lineIndex];

            // Alternate fade direction for each line
            int fadeWidth;
            int startX, endX;
            if (lineIndex % 2 == 0) {
                // Fade from left to right
                fadeWidth = fadeFrame;
                startX = 0;
                endX = std::min(winWidth, fadeWidth);
            } else {
                // Fade from right to left
                fadeWidth = fadeFrame;
                startX = std::max(0, winWidth - fadeWidth);
                endX = winWidth;
            }

            for (int x = startX; x < endX; ++x) {
                int textIndex =
                    ((offset + x) % textLength + textLength) % textLength;
                mvwaddch(context.window, y, x, text[textIndex]);
            }
        }
        wattroff(context.window, COLOR_PAIR(grey));

        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(fadeDrawTime));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_BEAT));

    // Choose a random dancer for the overlay
    int dancerIndex = context.rng() % dancers.size();
    const AsciiArt &dancer = dancers[dancerIndex];
    Gradient gradient = getRandomGradient(context.rng);
    int dancerColorIndex =
        getColourIndex(gradient, context.rng() % GRADIENT_LENGTH);

    // Position dancer in center
    int dancerY = (winHeight - dancer.getHeight()) / 2;
    int dancerX = (winWidth - dancer.getWidth()) / 2;

    // Fade in dancer from bottom to top
    int dancerDrawTime = MS_PER_BEAT / dancer.getHeight();
    wattron(context.window, COLOR_PAIR(dancerColorIndex));
    for (int y = dancer.getHeight() - 1; y >= 0; --y) {
        for (int x = 0; x < dancer.getWidth(); ++x) {
            char c = dancer.getChar(x, y);
            if (c != ' ') {
                int screenY = dancerY + y;
                int screenX = dancerX + x;
                if (screenY >= 0 && screenY < winHeight && screenX >= 0 &&
                    screenX < winWidth) {
                    mvwaddch(context.window, screenY, screenX, c);
                }
            }
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(dancerDrawTime));
    }
    wattroff(context.window, COLOR_PAIR(dancerColorIndex));

    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_BEAT));

    int totalFrames = 1500;
    for (int frame = 0; frame < totalFrames; ++frame) {
        werase(context.window);

        // Every 16 frames, randomly shift one line
        if (frame % 16 == 0 && !lineOffsets.empty()) {
            int randomLine = context.rng() % lineOffsets.size();
            int direction = (context.rng() % 2 == 0) ? -1 : 1;
            lineOffsets[randomLine] += direction;
        }

        // Draw text on alternating lines
        wattron(context.window, COLOR_PAIR(grey));
        for (int y = 0, lineIndex = 0; y < winHeight; y += 2, ++lineIndex) {
            if (lineIndex >= lineOffsets.size())
                break;

            int offset = lineOffsets[lineIndex];
            for (int x = 0; x < winWidth; ++x) {
                int textIndex =
                    ((offset + x) % textLength + textLength) % textLength;
                mvwaddch(context.window, y, x, text[textIndex]);
            }
        }
        wattroff(context.window, COLOR_PAIR(grey));

        // Draw dancer with subtle wiggle and ripple effect
        wattron(context.window, COLOR_PAIR(dancerColorIndex));

        for (int y = 0; y < dancer.getHeight(); ++y) {
            // Calculate ease value for 32 frame cycle per row
            float cycleProgress = ((frame + y) % 32) /
                                  32.0f;  // 0-1 over 32 frames, offset by row
            float ease = 0.0f;

            if (cycleProgress >
                0.5f) {  // After first 16 frames (0.5 * 32 = 16)
                float secondHalfProgress =
                    (cycleProgress - 0.5f) * 2.0f;  // 0-1 over second half

                if (secondHalfProgress < 0.5f) {
                    // First 8 frames of second half: ease from 0 to 1
                    ease = secondHalfProgress * 2.0f;
                } else {
                    // Last 8 frames of second half: ease from 1 to 0
                    ease = (1.0f - secondHalfProgress) * 2.0f;
                }
            }

            // Calculate wiggle for this row (each row has different phase)
            float wiggleOffset =
                sin((frame + y) * 0.5f) * 4.0f * ease;  // Apply ease
            int wiggleX = (int)wiggleOffset;

            for (int x = 0; x < dancer.getWidth(); ++x) {
                char c = dancer.getChar(x, y);
                if (c != ' ') {
                    int screenY = dancerY + y;
                    int screenX =
                        dancerX + x + wiggleX;  // Apply row-specific wiggle

                    if (screenY >= 0 && screenY < winHeight && screenX >= 0 &&
                        screenX < winWidth) {
                        mvwaddch(context.window, screenY, screenX, c);
                    }
                }
            }
        }
        wattroff(context.window, COLOR_PAIR(dancerColorIndex));

        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }
}
