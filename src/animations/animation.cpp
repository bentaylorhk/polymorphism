/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "animation.h"

#include <ncurses.h>

#include <future>
#include <string>

void Animation::run(const AnimationContext &context) {
    try {
        context.logger->info("Running animation: {}", name());

        curs_set(FALSE);

        auto start = std::chrono::steady_clock::now();

        auto fut = std::async(std::launch::async, [&]() {
            this->drawFrame(context);  // Call the subclass's implementation
        });

        // Clearing screen if meant to start blank
        if (startState == TransitionState::Blank) {
            werase(context.window);
            wrefresh(context.window);
        }

        if (fut.wait_for(MAX_ANIMATION_DURATION) ==
            std::future_status::timeout) {
            context.logger->error("Animation '{}' timed out!", name());
        } else {
            fut.get();  // Re-throw exceptions if any
        }

        // Hard resetting formatting after each animation
        wattron(context.window, A_NORMAL);
        wattron(context.window, COLOR_PAIR(0));

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> sec = end - start;
        context.logger->info("Animation '{}' completed in {:.3f} s", name(),
                             sec.count());

    } catch (const std::exception &e) {
        context.logger->error("Exception in animation '{}': {}", name(),
                              e.what());
    } catch (...) {
        context.logger->error("Unknown exception in animation '{}'.", name());
    }
}