/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "neofetch.h"

#include <ncurses.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <vector>

// Helper to strip ANSI escape codes (for fallback if color not supported)
static std::string strip_ansi(const std::string &input) {
    std::string out;
    bool in_escape = false;
    for (size_t i = 0; i < input.size(); ++i) {
        if (!in_escape && input[i] == '\033')
            in_escape = true;
        else if (in_escape && input[i] == 'm')
            in_escape = false;
        else if (!in_escape)
            out += input[i];
    }
    return out;
}

void Neofetch::drawFrame(const AnimationContext &context) {
    // Build the command
    std::string cmd =
        "neofetch --source ~/repos/polyphonic-rsvp/ascii-art/neofetch_logo.txt";
    //"--stdout 2>/dev/null";
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        return;
    std::vector<std::string> lines;
    char buf[4096];
    while (fgets(buf, sizeof(buf), pipe)) {
        lines.emplace_back(buf);
    }
    pclose(pipe);
    // Try to print with color (if supported)
    // Ncurses can't natively parse ANSI, so fallback to no color
    werase(context.window);
    int max_y, max_x;
    getmaxyx(context.window, max_y, max_x);
    int start_y = (max_y - (int)lines.size()) / 2;
    for (size_t i = 0; i < lines.size(); ++i) {
        std::string clean = strip_ansi(lines[i]);
        mvwprintw(context.window, start_y + i, 2, "%s", clean.c_str());
    }
    wrefresh(context.window);
    std::this_thread::sleep_for(std::chrono::seconds(6));
}
