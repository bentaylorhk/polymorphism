/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include <ncurses.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <chrono>
#include <deque>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "animations/animations.h"
#include "util/colours.h"
#include "util/common.h"

constexpr int PADDING_TOP = 1;
constexpr int PADDING_BOTTOM = 0;
constexpr int PADDING_LEFT = 2;
constexpr int PADDING_RIGHT = 4;

void loop(AnimationContext &context, std::optional<int> count = std::nullopt) {
    std::map<TransitionState, std::vector<std::shared_ptr<Animation>>>
        animationsStartMap = getAnimationsByStartState();

    std::deque<std::string> recentNames;
    constexpr size_t RECENT_LIMIT = 6;

    auto randomAnimation =
        [&](TransitionState startState) -> std::shared_ptr<Animation> {
        std::vector<std::shared_ptr<Animation>> vec =
            animationsStartMap[startState];
        if (vec.empty()) {
            context.logger->error("No animations available for start state: {}",
                                  static_cast<int>(startState));
            return nullptr;
        }
        // Filter out recent animations if possible
        std::vector<std::shared_ptr<Animation>> filtered;
        for (const auto &anim : vec) {
            if (std::find(recentNames.begin(), recentNames.end(),
                          anim->name()) == recentNames.end())
                filtered.push_back(anim);
        }
        // If all are filtered out, fallback to full list
        const auto &pickFrom = filtered.empty() ? vec : filtered;
        std::uniform_int_distribution<size_t> dist(0, pickFrom.size() - 1);
        auto chosen = pickFrom[dist(context.rng)];
        context.logger->debug("Selected animation: {} (start state: {})",
                              chosen->name(), static_cast<int>(startState));
        return chosen;
    };

    // Start with "single cascade" animation
    std::shared_ptr<Animation> currentAnimation =
        findAnimationByName("single-cascade");
    if (!currentAnimation) {
        context.logger->error(
            "Animation 'single cascade' not found. Falling back to random.");
        currentAnimation = randomAnimation(TransitionState::Blank);
    }

    int played = 0;
    while (!count || played < *count) {
        currentAnimation->run(context);

        // Track recent animations
        recentNames.push_back(currentAnimation->name());
        if (recentNames.size() > RECENT_LIMIT) {
            recentNames.pop_front();
        }

        // Sleep if ending on Blank
        if (currentAnimation->endState == TransitionState::Blank) {
            context.logger->debug(
                "Sleeping for 1 second after blank transition.");
            std::this_thread::sleep_for(
                std::chrono::milliseconds(MS_PER_DOUBLE_BEAT));
        }
        currentAnimation = randomAnimation(currentAnimation->endState);
        ++played;
    }
}

int main(int argc, char *argv[]) {
    // Create logger only once
    auto logger =
        spdlog::basic_logger_mt("polyphonic_logger", "polyphonic.log");
    logger->set_level(spdlog::level::debug);
    logger->flush_on(spdlog::level::info);

    CLI::App app{
        "Polyphonic RSVP: A collection of ASCII art animations made for the "
        "event 'Polyphonic RSVP'."};

    std::string animationName;
    app.add_option("--animation", animationName,
                   "Play a specific animation by name");

    std::string sourceDir = "/home/ben/repos/polyphonic-rsvp/src";
    app.add_option("--source-dir", sourceDir,
                   "The source directory for the project files")
        ->default_val(sourceDir)
        ->check(CLI::ExistingDirectory);

    std::optional<int> count;
    app.add_option(
        "--count", count,
        "Number of animations to play before exiting (default: infinite)");

    CLI11_PARSE(app, argc, argv);

    initscr();
    noecho();
    curs_set(FALSE);

    setupColours();

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

    // Create animation context with logger
    AnimationContext context{subwindow, sourceDir, rng, logger};

    if (!animationName.empty()) {
        logger->info("Requested specific animation: {}", animationName);
        // Play a specific animation by name
        std::shared_ptr<Animation> animation =
            findAnimationByName(animationName);
        if (!animation) {
            delwin(subwindow);
            endwin();
            logger->error("Animation '{}' not found. Exiting.", animationName);
            std::cerr << "Error: Animation '" << animationName
                      << "' not found. Available animations are:";
            for (const auto &animation : allAnimations) {
                std::cerr << " " << animation->name() << ",";
            }
            std::cerr << std::endl;
            return EXIT_FAILURE;
        }

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
        logger->info("Starting animation loop");
        loop(context, count);
    }

    curs_set(TRUE);
    delwin(subwindow);
    endwin();
    logger->info("Exited cleanly.");
    return EXIT_SUCCESS;
}
