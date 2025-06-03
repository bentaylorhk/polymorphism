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
#include "util/colours.h"

constexpr int PADDING_TOP = 1;
constexpr int PADDING_BOTTOM = 0;
constexpr int PADDING_LEFT = 2;
constexpr int PADDING_RIGHT = 4;

void loop(AnimationContext &context) {
    std::map<TransitionState, std::vector<std::shared_ptr<Animation>>>
        animationsStartMap = getAnimationsByStartState();

    auto randomAnimation =
        [&](TransitionState startState) -> std::shared_ptr<Animation> {
        std::vector<std::shared_ptr<Animation>> vec =
            animationsStartMap[startState];
        if (vec.empty())
            return nullptr;
        std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
        return vec[dist(context.rng)];
    };

    auto currentAnimation = randomAnimation(TransitionState::Blank);

    while (true) {
        currentAnimation->run(context);
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

    setupColours();

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

    // Creating a padded window to suit overscanned CRT
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Calculate dimensions for the subwindow
    int win_height = max_y - PADDING_TOP - PADDING_BOTTOM;
    int win_width = max_x - PADDING_LEFT - PADDING_RIGHT;

    // Create the subwindow with position and size
    WINDOW *subwindow =
        newwin(win_height, win_width, PADDING_TOP, PADDING_LEFT);

    std::random_device rd;
    std::mt19937 rng(static_cast<std::mt19937::result_type>(rd()));

    // Create animation context
    AnimationContext context{subwindow, asciiArt, sourceDir, rng};

    if (!animationName.empty()) {
        // Play a specific animation by name
        std::shared_ptr<Animation> animation =
            findAnimationByName(animationName);
        if (!animation) {
            delwin(subwindow);
            endwin();
            std::cerr << "Error: Animation '" << animationName
                      << "' not found. Available animations are:";
            for (const auto &animation : allAnimations) {
                std::cerr << " " << animation->name() << ",";
            }
            std::cerr << std::endl;
            return EXIT_FAILURE;
        }

        // TODO: Potentially delete on main run.
        if (animation->startState == TransitionState::Anything) {
            std::bernoulli_distribution dist(0.25);
            for (int y = 0; y < win_height; ++y) {
                for (int x = 0; x < win_width; ++x) {
                    if (dist(context.rng)) {
                        mvwaddch(context.window, y, x, '@');
                    }
                }
            }
            wrefresh(context.window);
        }

        animation->run(context);
    } else {
        loop(context);
    }

    curs_set(TRUE);
    delwin(subwindow);
    endwin();
    return EXIT_SUCCESS;
}
