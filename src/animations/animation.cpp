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
        } else {
            fut.get();  // Re-throw exceptions if any
        }

        // Hard resetting formatting after each animation
        wattron(context.window, A_NORMAL);
        wattron(context.window, COLOR_PAIR(0));
    } catch (const std::exception &e) {
    } catch (...) {
    }
}