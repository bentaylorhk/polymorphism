/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "colour_preview.h"

#include <ncurses.h>

#include <string>
#include <thread>

#include "../util/colours.h"
#include "../util/common.h"  // for intensityChars, easeInOutQuad

void ColourPreview::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    werase(context.window);

    auto gradients = getAllRandomGradients(context.rng);
    int N = gradients.size();
    int sectionHeight = winHeight / N;
    int remainder = winHeight % N;
    int yStart = 0;
    bool leftToRight = true;

    // Lambda to fill a section with animated chars
    auto fillSection = [&](int ySectionStart, int ySectionEnd,
                           Gradient gradient, bool ltr, char fillChar) {
        for (int step = 0; step < winWidth; ++step) {
            int col = ltr ? step : (winWidth - 1 - step);
            for (int row = ySectionStart; row < ySectionEnd && row < winHeight;
                 ++row) {
                int gradIdx = (col * GRADIENT_LENGTH) / winWidth;
                if (gradIdx >= GRADIENT_LENGTH)
                    gradIdx = GRADIENT_LENGTH - 1;
                int colourPair = getColourIndex(gradient, gradIdx);
                wattron(context.window, COLOR_PAIR(colourPair));
                mvwaddch(context.window, row, col, fillChar);
                wattroff(context.window, COLOR_PAIR(colourPair));
            }
            wrefresh(context.window);
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    };

    // 1. Fill each section with '.' (each section fills only its band)
    yStart = 0;
    for (int idx = 0; idx < N; ++idx) {
        int thisSectionHeight = sectionHeight + (idx < remainder ? 1 : 0);
        int ySectionStart = yStart;
        int ySectionEnd = yStart + thisSectionHeight;
        Gradient gradient = gradients[idx];
        fillSection(ySectionStart, ySectionEnd, gradient, leftToRight, '.');
        leftToRight = !leftToRight;
        yStart += thisSectionHeight;
    }
    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    // 2. Animate all cells to randomly increase to a peak intensity char (all
    // sections at once, each fills only its band)
    int upFrames = 18;
    yStart = 0;
    for (int frame = 0; frame <= upFrames; ++frame) {
        float intensity = easeInOutQuad((float)frame / upFrames);  // 0 to 1
        yStart = 0;
        for (int idx = 0; idx < N; ++idx) {
            int thisSectionHeight = sectionHeight + (idx < remainder ? 1 : 0);
            int ySectionStart = yStart;
            int ySectionEnd = yStart + thisSectionHeight;
            Gradient gradient = gradients[idx];
            for (int row = ySectionStart; row < ySectionEnd && row < winHeight;
                 ++row) {
                for (int col = 0; col < winWidth; ++col) {
                    int gradIdx = (col * GRADIENT_LENGTH) / winWidth;
                    if (gradIdx >= GRADIENT_LENGTH)
                        gradIdx = GRADIENT_LENGTH - 1;
                    int colourPair = getColourIndex(gradient, gradIdx);
                    int maxIdx =
                        std::round(intensity * (intensityChars.size() - 1));
                    int idxNow = 0;
                    if (maxIdx == 0) {
                        idxNow = 0;  // always '.'
                    } else {
                        std::uniform_int_distribution<int> dist(0, maxIdx);
                        idxNow = dist(context.rng);
                    }
                    wattron(context.window, COLOR_PAIR(colourPair));
                    mvwaddch(context.window, row, col, intensityChars[idxNow]);
                    wattroff(context.window, COLOR_PAIR(colourPair));
                }
            }
            yStart += thisSectionHeight;
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 3. Fade all cells back to '.' and then to blank, except POLYPHONIC (all
    // sections at once, each fills only its band)
    int downFrames = 18;
    int wordLen = polyphonic.size();
    for (int frame = 0; frame <= downFrames; ++frame) {
        float intensity = 1.0f - easeInOutQuad((float)frame / downFrames);
        yStart = 0;
        for (int idx = 0; idx < N; ++idx) {
            int thisSectionHeight = sectionHeight + (idx < remainder ? 1 : 0);
            int ySectionStart = yStart;
            int ySectionEnd = yStart + thisSectionHeight;
            Gradient gradient = gradients[idx];
            int xStart = (winWidth - wordLen) / 2;
            int yMid = ySectionStart + thisSectionHeight / 2;
            for (int row = ySectionStart; row < ySectionEnd && row < winHeight;
                 ++row) {
                for (int col = 0; col < winWidth; ++col) {
                    // Leave POLYPHONIC in the center row
                    bool isPolyphonic = (row == yMid && col >= xStart &&
                                         col < xStart + wordLen);
                    if (isPolyphonic && frame == downFrames) {
                        int i = col - xStart;
                        int gradIdx = (i * GRADIENT_LENGTH) / wordLen;
                        if (gradIdx >= GRADIENT_LENGTH)
                            gradIdx = GRADIENT_LENGTH - 1;
                        int colourPair = getColourIndex(gradient, gradIdx);
                        wattron(context.window, COLOR_PAIR(colourPair));
                        mvwaddch(context.window, row, col, polyphonic[i]);
                        wattroff(context.window, COLOR_PAIR(colourPair));
                        continue;
                    }
                    int gradIdx = (col * GRADIENT_LENGTH) / winWidth;
                    if (gradIdx >= GRADIENT_LENGTH)
                        gradIdx = GRADIENT_LENGTH - 1;
                    int colourPair = getColourIndex(gradient, gradIdx);
                    int maxIdx =
                        std::round(intensity * (intensityChars.size() - 1));
                    int idxNow = 0;
                    if (frame == downFrames) {
                        idxNow = 0;  // blank
                    } else if (maxIdx == 0) {
                        idxNow = 0;  // always '.'
                    } else {
                        std::uniform_int_distribution<int> dist(0, maxIdx);
                        idxNow = dist(context.rng);
                    }
                    char c =
                        (frame == downFrames) ? ' ' : intensityChars[idxNow];
                    wattron(context.window, COLOR_PAIR(colourPair));
                    mvwaddch(context.window, row, col, c);
                    wattroff(context.window, COLOR_PAIR(colourPair));
                }
            }
            // On last frame, draw POLYPHONIC again to ensure it's visible
            if (frame == downFrames) {
                for (int i = 0; i < wordLen && (xStart + i) < winWidth; ++i) {
                    int gradIdx = (i * GRADIENT_LENGTH) / wordLen;
                    if (gradIdx >= GRADIENT_LENGTH)
                        gradIdx = GRADIENT_LENGTH - 1;
                    int colourPair = getColourIndex(gradient, gradIdx);
                    wattron(context.window, COLOR_PAIR(colourPair));
                    mvwaddch(context.window, yMid, xStart + i, polyphonic[i]);
                    wattroff(context.window, COLOR_PAIR(colourPair));
                }
            }
            yStart += thisSectionHeight;
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
    werase(context.window);
    wrefresh(context.window);
}