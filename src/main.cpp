/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include <ncurses.h>

#include <CLI/CLI.hpp>
#include <chrono>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "animations/animations.h"
#include "common.h"

void loop(AnimationContext &context) {
    std::random_device rd;
    std::mt19937 rng(rd());

    std::map<TransitionState, std::vector<std::shared_ptr<Animation>>>
        animationsStartMap = getAnimationsByStartState();

    auto randomAnimation =
        [&rng, &animationsStartMap](
            TransitionState startState) -> std::shared_ptr<Animation> {
        std::vector<std::shared_ptr<Animation>> vec =
            animationsStartMap[startState];
        if (vec.empty())
            return nullptr;
        std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
        return vec[dist(rng)];
    };

    auto currentAnimation = randomAnimation(TransitionState::Blank);

    while (true) {
        currentAnimation->run(context);

        // Wait for user input to skip or quit
        /*
        TODO Figure out how to handle user input, probs through
        TODO This logic will also likely handle timeout issues
        interrupts/curses. int ch = getch(); if (ch == ' ') {
            // Skip to the next animation
            continue;
        } else if (ch == 'q') {
            // Quit the program
            break;
        }
        */

        // Getting the next animation randomly based off it's end state
        currentAnimation = randomAnimation(currentAnimation->endState);
    }
}

int main(int argc, char *argv[]) {
    CLI::App app{
        "Polyphonic RSVP: A collection of ASCII art animations made for the "
        "event 'Polyphonic RSVP'."};

    std::string logoFilePath =
        "/home/ben/repos/polyphonic-rsvp/ascii-art/POLYPHONIC_DOLLAR.txt";
    app.add_option("--logo", logoFilePath, "Path to the ASCII logo file")
        ->default_val(logoFilePath)
        ->check(CLI::ExistingFile);

    std::string animationName;
    app.add_option("--animation", animationName,
                   "Play a specific animation by name");

    std::string sourceDir = "/home/ben/repos/polyphonic-rsvp/src";
    app.add_option("--source-dir", sourceDir,
                   "The source directory for the project files")
        ->default_val(sourceDir)
        ->check(CLI::ExistingDirectory);

    CLI11_PARSE(app, argc, argv);

    initscr();
    noecho();
    curs_set(FALSE);
    start_color();
    use_default_colors();

    init_pair(1, COLOR_MAGENTA, -1);
    init_pair(2, COLOR_CYAN, -1);
    init_pair(3, COLOR_GREEN, -1);
    init_pair(4, COLOR_YELLOW, -1);

    // Load ASCII art
    std::ifstream file(logoFilePath);
    if (!file.is_open()) {
        endwin();
        throw std::runtime_error("Failed to open ASCII art file: " +
                                 logoFilePath);
    }
    std::string rawArt((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    // 2D array of chars, for easy iteration
    int artHeight, artWidth;
    getStringDimensions(rawArt, artWidth, artHeight);
    AsciiArt asciiArt = stringTo2DArray(rawArt, artWidth, artHeight);

    // Create animation context
    AnimationContext context{stdscr, asciiArt, sourceDir};

    if (!animationName.empty()) {
        // Play a specific animation by name
        std::shared_ptr<Animation> animation =
            findAnimationByName(animationName);
        if (!animation) {
            endwin();
            std::cerr << "Error: Animation '" << animationName
                      << "' not found. Available animations are:";
            for (const auto &animation : allAnimations) {
                std::cerr << " " << animation->name();
            }
            std::cerr << std::endl;
            return EXIT_FAILURE;
        }
        animation->run(context);
    } else {
        loop(context);
    }

    curs_set(TRUE);
    endwin();
    return EXIT_SUCCESS;
}
