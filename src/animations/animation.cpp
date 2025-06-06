/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#include "animation.h"

#include <ncurses.h>

#include <future>
#include <string>

void Animation::run(const AnimationContext &context) {
    // Hard resetting formatting before each animation
    wattron(context.window, A_NORMAL);
    wattron(context.window, COLOR_PAIR(0));
    curs_set(FALSE);

    // Clearing screen if meant to start blank
    if (startState == TransitionState::Blank) {
        werase(context.window);
        wrefresh(context.window);
    }

    this->drawFrame(context);
}