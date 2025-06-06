/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "colour_preview.h"

#include <ncurses.h>

#include <string>
#include <thread>

#include "../util/colours.h"
#include "../util/common.h"

const std::vector<char> PREVIEW_CHARS = {'=', 'x', '*', '#', 'X', '$', '@'};

void ColourPreview::drawFrame(const AnimationContext &context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);

    werase(context.window);
    wrefresh(context.window);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MS_PER_SIXTY_FOURTH_BEAT));

    auto gradients = getAllRandomGradients(context.rng);
    int N = gradients.size();
    int sectionHeight = 3;  // Each gradient row is 3 cells tall
    int totalGradientRows =
        N * sectionHeight + (N - 1);  // (N-1) empty rows between
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
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_SIXTY_FOURTH_BEAT));
        }
    };

    // Lambda to draw POLYPHONIC across the entire row, repeating as needed
    auto drawPolyphonic = [&](int row, int winWidth, Gradient gradient,
                              bool always) {
        int wordLen = polyphonic.size();
        for (int i = 0; i < winWidth; ++i) {
            int gradIdx = (i * GRADIENT_LENGTH) / winWidth;
            if (gradIdx >= GRADIENT_LENGTH)
                gradIdx = GRADIENT_LENGTH - 1;
            int colourPair = getColourIndex(gradient, gradIdx);
            // Only draw if always==true (last frame) or if char is blank
            chtype ch = mvwinch(context.window, row, i);
            if (always || (ch & A_CHARTEXT) == '=') {
                wattron(context.window, COLOR_PAIR(colourPair));
                mvwaddch(context.window, row, i, polyphonic[i % wordLen]);
                wattroff(context.window, COLOR_PAIR(colourPair));
            }
        }
    };

    yStart = 0;
    for (int idx = 0; idx < N; ++idx) {
        int ySectionStart = yStart;
        int ySectionEnd = yStart + sectionHeight;
        Gradient gradient = gradients[idx];
        fillSection(ySectionStart, ySectionEnd, gradient, leftToRight,
                    PREVIEW_CHARS[0]);
        leftToRight = !leftToRight;
        yStart += sectionHeight;
        // Add a single empty row between gradient rows, except after the last
        if (idx < N - 1 && yStart < winHeight) {
            for (int col = 0; col < winWidth; ++col)
                mvwaddch(context.window, yStart, col, ' ');
            ++yStart;
        }
    }
    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));

    // 2. Animate all cells to randomly increase to a peak intensity char (all
    // sections at once, each fills only its band)
    int upFrames = 75;
    // We'll need to skip the empty rows in all animation loops
    for (int frame = 0; frame <= upFrames; ++frame) {
        float progress = (float)frame / upFrames;
        float intensity;
        if (progress < 0.5f) {
            intensity =
                easeInOutQuad(progress / 0.5f);  // 0 to 1 over first half
        } else {
            intensity = 1.0f;
        }
        yStart = 0;
        for (int idx = 0; idx < N; ++idx) {
            int ySectionStart = yStart;
            int ySectionEnd = yStart + sectionHeight;
            Gradient gradient = gradients[idx];
            for (int row = ySectionStart; row < ySectionEnd && row < winHeight;
                 ++row) {
                for (int col = 0; col < winWidth; ++col) {
                    int gradIdx = (col * GRADIENT_LENGTH) / winWidth;
                    if (gradIdx >= GRADIENT_LENGTH)
                        gradIdx = GRADIENT_LENGTH - 1;
                    int colourPair = getColourIndex(gradient, gradIdx);
                    int maxIdx =
                        std::round(intensity * (PREVIEW_CHARS.size() - 1));
                    int idxNow = 0;
                    if (maxIdx == 0) {
                        idxNow = 0;  // always .
                    } else {
                        std::uniform_int_distribution<int> dist(0, maxIdx);
                        idxNow = dist(context.rng);
                    }
                    wattron(context.window, COLOR_PAIR(colourPair));
                    mvwaddch(context.window, row, col, PREVIEW_CHARS[idxNow]);
                    wattroff(context.window, COLOR_PAIR(colourPair));
                }
            }
            yStart += sectionHeight;
            // Skip the empty row
            if (idx < N - 1 && yStart < winHeight)
                ++yStart;
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }

    // 3. Fade all cells back to '.' and then to blank, except POLYPHONIC (all
    // sections at once, each fills only its band)
    int downFrames = 40;
    int wordLen = polyphonic.size();
    for (int frame = 0; frame <= downFrames; ++frame) {
        float intensity = 1.0f - easeInOutQuad((float)frame / downFrames);
        yStart = 0;
        for (int idx = 0; idx < N; ++idx) {
            int ySectionStart = yStart;
            int ySectionEnd = yStart + sectionHeight;
            Gradient gradient = gradients[idx];
            int yMid = ySectionStart + sectionHeight / 2;
            for (int row = ySectionStart; row < ySectionEnd && row < winHeight;
                 ++row) {
                for (int col = 0; col < winWidth; ++col) {
                    int gradIdx = (col * GRADIENT_LENGTH) / winWidth;
                    if (gradIdx >= GRADIENT_LENGTH)
                        gradIdx = GRADIENT_LENGTH - 1;
                    int colourPair = getColourIndex(gradient, gradIdx);
                    int maxIdx =
                        std::round(intensity * (PREVIEW_CHARS.size() - 1));
                    int idxNow = 0;
                    if (frame == downFrames) {
                        idxNow = 0;  // blank
                    } else if (maxIdx == 0) {
                        idxNow = 0;  // always blank
                    } else {
                        std::uniform_int_distribution<int> dist(0, maxIdx);
                        idxNow = dist(context.rng);
                    }
                    char c =
                        (frame == downFrames) ? '=' : PREVIEW_CHARS[idxNow];
                    wattron(context.window, COLOR_PAIR(colourPair));
                    mvwaddch(context.window, row, col, c);
                    wattroff(context.window, COLOR_PAIR(colourPair));
                }
                // Draw POLYPHONIC on this row if needed
                if (row == yMid) {
                    drawPolyphonic(row, winWidth, gradient,
                                   frame == downFrames);
                }
            }
            yStart += sectionHeight;
            // Skip the empty row
            if (idx < N - 1 && yStart < winHeight)
                ++yStart;
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MS_PER_QUADRUPLE_BEAT));
    wrefresh(context.window);
}