/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

// TODO: Fix the input context window such that it's cropped to the screen size.
// TODO: Maybe prioritise better files, like this code_scroll and the main file

#include "code_scroll.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

std::vector<std::string> CodeScroll::findFiles(
    const std::string& sourceDir) const {
    std::vector<std::string> files;
    for (auto& p : std::filesystem::recursive_directory_iterator(sourceDir)) {
        if (!p.is_regular_file()) {
            continue;
        }
        std::string name = p.path().filename().string();
        if (name == "CMakeFiles.txt" || p.path().extension() == ".h" ||
            p.path().extension() == ".cpp") {
            files.push_back(p.path().string());
        }
    }
    return files;
}

void CodeScroll::run(const AnimationContext& context) {
    scrollok(context.window, TRUE);
    wclear(context.window);

    auto files = findFiles(context.sourceDir);
    if (files.empty()) {
        return;  // No files to scroll through
    }

    int rows, cols;
    context.getDimensions(cols, rows);

    int lastLine = rows - 1;

    auto scrollPrint = [&](const std::string& text) {
        scroll(context.window);

        // Truncate to window width to disable wrapping
        std::string displayText = text.substr(0, cols);

        // Case-insensitive search for "polyphonic"
        std::string lowerText = toLower(displayText);
        std::string word = "polyphonic";
        size_t pos = lowerText.find(word);

        if (pos != std::string::npos && has_colors()) {
            // Print up to the match
            mvwaddnstr(context.window, lastLine, 0, displayText.c_str(), pos);

            // Highlight "polyphonic"
            wattron(context.window, COLOR_PAIR(2) | A_BOLD);
            mvwaddnstr(context.window, lastLine, pos, displayText.c_str() + pos,
                       word.size());
            wattroff(context.window, COLOR_PAIR(2) | A_BOLD);

            // Print the rest of the line, if any
            int afterPos = pos + word.size();
            if ((size_t)afterPos < displayText.size()) {
                mvwaddnstr(context.window, lastLine, afterPos,
                           displayText.c_str() + afterPos,
                           displayText.size() - afterPos);
            }
        } else {
            mvwaddnstr(context.window, lastLine, 0, displayText.c_str(),
                       displayText.size());
        }

        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    };

    for (const auto& file : files) {
        std::ifstream inFile(file);
        if (!inFile.is_open()) {
            continue;  // Skip files that can't be opened
        }

        scrollPrint(file);

        std::string line;
        while (std::getline(inFile, line)) {
            scrollPrint(line);
        }

        // Gap between files
        scrollPrint("");
        scrollPrint("");
    }

    scrollok(context.window, FALSE);
}