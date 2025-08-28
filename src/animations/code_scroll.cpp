/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "code_scroll.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include "../util/colours.h"

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

void CodeScroll::drawFrame(const AnimationContext& context) {
    scrollok(context.window, TRUE);
    wclear(context.window);

    auto files = findFiles(context.sourceDir);

    // Only keep the specific files
    const std::vector<std::string> allowed = {"main.cpp", "code_scroll.cpp"};
    files.erase(
        std::remove_if(files.begin(), files.end(),
                       [&](const std::string& path) {
                           std::string name =
                               std::filesystem::path(path).filename().string();
                           return std::find(allowed.begin(), allowed.end(),
                                            name) == allowed.end();
                       }),
        files.end());

    if (files.empty()) {
        return;  // No files to scroll through
    }

    int rows, cols;
    context.getDimensions(rows, cols);

    int lastLine = rows - 1;

    int edgePad = 1;

    Gradient gradient = getRandomGradient(context.rng);
    int colourIndex = getColourIndex(gradient, 0);

    auto scrollPrint = [&](const std::string& text) {
        scroll(context.window);

        // Truncate to window width to disable wrapping, -1 to stop jittering
        // for large text
        std::string displayText = text.substr(0, cols - (edgePad * 2));

        // Case-insensitive search for "polyphonic"
        std::string lowerText = toLower(displayText);
        std::string word = "polyphonic";
        size_t pos = lowerText.find(word);

        if (pos != std::string::npos) {
            // Print up to the match
            mvwaddnstr(context.window, lastLine, edgePad, displayText.c_str(),
                       pos);

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
            mvwaddnstr(context.window, lastLine, edgePad, displayText.c_str(),
                       displayText.size());
        }

        wrefresh(context.window);
        std::this_thread::sleep_for(
            std::chrono::milliseconds(MS_PER_EIGHTH_BEAT));
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
