/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <ncurses.h>

#include <string>

#include "../common.h"

// Represents the different start and end conditions for an animation
enum class TransitionState { Blank, Anything, Logo };

struct AnimationContext {
    WINDOW *window;
    const AsciiArt &asciiArt;
    const std::string &sourceDir;
    void getDimensions(int &width, int &height) const {
        getmaxyx(window, height, width);
    }
};

class Animation {
   public:
    Animation(TransitionState startState, TransitionState endState)
        : startState(startState), endState(endState) {
    }

    virtual ~Animation() = default;
    virtual void run(const AnimationContext &context) = 0;

    virtual const char *name() const = 0;

    const TransitionState startState;
    const TransitionState endState;
};