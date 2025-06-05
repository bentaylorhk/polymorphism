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

        auto end = std::chrono::steady_clock::now();
        auto ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
                .count();
        context.logger->info("Animation '{}' completed in {} ms", name(), ms);

    } catch (const std::exception &e) {
        context.logger->error("Exception in animation '{}': {}", name(),
                              e.what());
    } catch (...) {
        context.logger->error("Unknown exception in animation '{}'.", name());
    }
}