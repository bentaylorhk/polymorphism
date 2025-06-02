/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "animation.h"

#include <ncurses.h>

#include <future>
#include <string>

void Animation::run(const AnimationContext &context) {
    // Run the implementation with a timeout using std::async
    auto fut = std::async(std::launch::async, [&]() {
        this->drawFrame(context);  // Call the subclass's implementation
    });

    if (fut.wait_for(MAX_ANIMATION_DURATION) == std::future_status::timeout) {
        // Timeout! You can show a message or clean up if needed
        // mvwprintw(context.window, 0, 0, "Animation timed out!");
        // wrefresh(context.window);
    } else {
        // TODO: Maybe delete this on the day?
        fut.get();  // Re-throw exceptions if any
    }
}