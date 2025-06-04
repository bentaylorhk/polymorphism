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
        // Run the implementation with a timeout using std::async
        auto fut = std::async(std::launch::async, [&]() {
            this->drawFrame(context);  // Call the subclass's implementation
        });

        if (fut.wait_for(MAX_ANIMATION_DURATION) ==
            std::future_status::timeout) {
            // mvwprintw(context.window, 0, 0, "Animation timed out!");
            // wrefresh(context.window);
        } else {
            // TODO: Maybe delete this on the day?
            fut.get();  // Re-throw exceptions if any
        }

        context.logger->info("Animation '{}' completed", name());
    } catch (const std::exception &e) {
        context.logger->error("Exception in animation '{}': {}", name(),
                              e.what());
    } catch (...) {
        context.logger->error("Unknown exception in animation '{}'.", name());
    }
}