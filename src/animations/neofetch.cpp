/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "neofetch.h"

#include <ncurses.h>

#include <sstream>
#include <string>
#include <vector>

#include "../util/colours.h"

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
Resolution - 320x240 @ 60.00Hz
WM - i3
Font - scientifica
CPU - AMD Ryzen 5 8500G @ 5.0GHz
GPU - AMD Radeon HD 8490
Memory - 14.71GiB
)";

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
            // Remove possible leading/trailing whitespace
            if (!line.empty() &&
                (line.find_first_not_of(" \t\n\r") != std::string::npos))
                art_lines.push_back(line);
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

    int left_pad = 2;
    int art_col_width = 22;
    int info_col_start = art_col_width + 2;
    int top_pad = 1;
    int art_top_pad = top_pad + 5;

    // Print ASCII art
    for (size_t i = 0;
         i < art_lines.size() && (art_top_pad + (int)i) < winHeight; ++i) {
        mvwprintw(context.window, art_top_pad + i, left_pad, "%s",
                  art_lines[i].c_str());
    }
    // Print info text
    for (size_t i = 0; i < info_lines.size() && (top_pad + (int)i) < winHeight;
         ++i) {
        mvwprintw(context.window, top_pad + i, info_col_start, "%s",
                  info_lines[i].c_str());
    }

    // Print colour gradients at the bottom
    int grad_y = winHeight - (int)gradients.size() - 1;
    int grad_x = info_col_start;
    for (size_t g = 0; g < gradients.size(); ++g) {
        for (int j = 0; j < GRADIENT_LENGTH; ++j) {
            int colourPair = getInverseColourIndex(static_cast<Gradient>(g), j);
            wattron(context.window, COLOR_PAIR(colourPair));
            mvwaddch(context.window, grad_y + g, grad_x + j, ' ');
            wattroff(context.window, COLOR_PAIR(colourPair));
        }
    }
    wrefresh(context.window);
    std::this_thread::sleep_for(
        std::chrono::milliseconds(MS_PER_OCTUPLE_BEAT * 2));
}
