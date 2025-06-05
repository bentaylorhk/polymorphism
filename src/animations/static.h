/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <ncurses.h>

#include "animation.h"

class Static : public Animation {
   public:
    Static() : Animation(TransitionState::Anything, TransitionState::Blank) {
    }

    const char *name() const override {
        return "static";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};