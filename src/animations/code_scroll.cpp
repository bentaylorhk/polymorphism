/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

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
        mvwprintw(context.window, lastLine, 0, "%s", text.c_str());
        wrefresh(context.window);
        std::this_thread::sleep_for(std::chrono::milliseconds(83));
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
        scrollPrint("\n");
    }

    scrollok(context.window, FALSE);
}