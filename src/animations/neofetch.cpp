/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "neofetch.h"

#include <ncurses.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../util/colours.h"
#include "../util/common.h"

constexpr const char* ASCII_ART = R"(
    P   O   L   Y

  .g8""8q.   .M"""bgd
.dP'    `YM.,MI    "Y
dM'      `MM`MMb.
MM        MM  `YMMNq.
MM.      ,MP.     `MM
`Mb.    ,dP'Mb     dM
  `"bmmd"'  P"Ybmmd"
)";

constexpr const char* INFO_TEXT = R"(
ben @ polyphonic-1
==================
OS - polyOS
Kernel - 6.14.9-arch1-1
Packages - 770 (pacman)
Shell - /bin/bash 5.2.37
Display - Sony PVM-14L5
Resolution - 320x240 @ 60.08Hz
Font - scientifica
CPU - AMD Ryzen 5 8500G @ 5GHz
GPU - AMD Radeon HD 8490
Memory - 14.71GiB
)";

constexpr int COLOUR_BAR_WIDTH = 6;

void Neofetch::drawFrame(const AnimationContext& context) {
    int winHeight, winWidth;
    context.getDimensions(winHeight, winWidth);
    werase(context.window);

    // Split ASCII_ART and INFO_TEXT into lines
    std::vector<std::string> art_lines, info_lines;
    {
        std::istringstream art_stream(ASCII_ART);
        std::string line;
        while (std::getline(art_stream, line)) {
            // Only skip lines that are *completely* whitespace
            if (line.find_first_not_of(" \t\n\r") == std::string::npos) {
                art_lines.push_back("");  // preserve blank line
            } else {
                art_lines.push_back(line);
            }
        }
    }
    {
        std::istringstream info_stream(INFO_TEXT);
        std::string line;
        while (std::getline(info_stream, line)) {
            if (!line.empty() &&
                (line.find_first_not_of(" \t\n\r") != std::string::npos))
                info_lines.push_back(line);
        }
    }

    // Calculate dimensions for layout
    int artWidth = 0, artHeight = 0;
    getStringDimensions(ASCII_ART, artWidth, artHeight);
    int infoWidth = 0, infoHeight = 0;
    getStringDimensions(INFO_TEXT, infoWidth, infoHeight);

    auto randomGradients = getAllRandomGradients(context.rng);
    int colourBarHeight = (int)randomGradients.size();
    int colourBarLen = GRADIENT_LENGTH * COLOUR_BAR_WIDTH;

    int infoAndBarHeight = infoHeight + colourBarHeight + 1;
    int blockHeight = std::max(artHeight, infoAndBarHeight);
    int blockTopPad = (winHeight - blockHeight) / 2;

    int artColX = (winWidth - (artWidth + infoWidth + 4)) / 2;
    if (artColX < 0)
        artColX = 0;
    int infoColX = artColX + artWidth + 4;

    int artTopPad = blockTopPad + (blockHeight - artHeight) / 2;
    int infoTopPad = blockTopPad + (blockHeight - infoAndBarHeight) / 2;

    // Print ASCII art
    for (size_t i = 0; i < art_lines.size() && (artTopPad + (int)i) < winHeight;
         ++i) {
        mvwprintw(context.window, artTopPad + i, artColX, "%s",
                  art_lines[i].c_str());
    }
    // Print info text
    for (size_t i = 0;
         i < info_lines.size() && (infoTopPad + (int)i) < winHeight; ++i) {
        mvwprintw(context.window, infoTopPad + i, infoColX, "%s",
                  info_lines[i].c_str());
    }

    // Fade in ASCII art
    int fadeFrames = 12;
    for (int frame = 0; frame <= fadeFrames; ++frame) {
        float progress = (float)frame / fadeFrames;
        int linesToShow = std::round(progress * art_lines.size());
        werase(context.window);
        for (int i = 0; i < linesToShow && (artTopPad + i) < winHeight; ++i) {
            mvwprintw(context.window, artTopPad + i, artColX, "%s",
                      art_lines[i].c_str());
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_SIXTEENTH_BEAT));
    }

    // Animate info text
    curs_set(TRUE);
    int infoX = infoColX;
    std::exponential_distribution<double> typingDelayDist(1.0 / 20.0);
    for (size_t i = 0;
         i < info_lines.size() && (infoTopPad + (int)i) < winHeight; ++i) {
        int lineY = infoTopPad + i;
        int lineX = infoX;
        const std::string& line = info_lines[i];
        for (size_t c = 0; c < line.size(); ++c) {
            mvwaddch(context.window, lineY, lineX + c, line[c]);
            wmove(context.window, lineY, lineX + c + 1);
            wrefresh(context.window);
            int delay = std::clamp(
                static_cast<int>(typingDelayDist(context.rng)), 10, 60);
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));
    curs_set(FALSE);

    // Animate gradient bars
    int gradY = infoTopPad + infoHeight + 1;
    int gradX = infoColX;
    for (int col = 0; col < colourBarLen; ++col) {
        for (size_t g = 0; g < randomGradients.size(); ++g) {
            int gradIdx = col / COLOUR_BAR_WIDTH;
            if (gradIdx >= GRADIENT_LENGTH)
                continue;
            int colourPair = getInverseColourIndex(randomGradients[g], gradIdx);
            wattron(context.window, COLOR_PAIR(colourPair));
            mvwaddch(context.window, gradY + g, gradX + col, ' ');
            wattroff(context.window, COLOR_PAIR(colourPair));
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_THIRTY_SECOND_BEAT));
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MS_PER_QUADRUPLE_BEAT));

    // Fade out gradient bars
    for (int col = 0; col < colourBarLen; ++col) {
        for (size_t g = 0; g < randomGradients.size(); ++g) {
            int gradIdx = col / COLOUR_BAR_WIDTH;
            if (gradIdx >= GRADIENT_LENGTH)
                continue;
            mvwaddch(context.window, gradY + g, gradX + col, ' ');
        }
        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_THIRTY_SECOND_BEAT));
    }
    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::milliseconds(MS_PER_BEAT));
}
