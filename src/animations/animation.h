/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <ncurses.h>

#include <chrono>
#include <string>

#include "../common.h"

constexpr auto MAX_ANIMATION_DURATION = std::chrono::seconds(30);

// Represents the different start and end conditions for an animation
enum class TransitionState { Blank, Anything, Logo };

struct AnimationContext {
    WINDOW *window;
    const AsciiArt &asciiArt;
    const std::string &sourceDir;
    inline void getDimensions(int &height, int &width) const {
        getmaxyx(window, height, width);
    }
};

class Animation {
   public:
    Animation(TransitionState startState, TransitionState endState)
        : startState(startState), endState(endState) {
    }

    virtual ~Animation() = default;
    void run(const AnimationContext &context);

    virtual const char *name() const = 0;

    const TransitionState startState;
    const TransitionState endState;

   protected:
    virtual void drawFrame(const AnimationContext &context) = 0;
};