/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class ScrollingPolyphonic : public Animation {
   public:
    ScrollingPolyphonic()
        : Animation(TransitionState::Blank, TransitionState::Anything) {
    }
    const char *name() const override {
        return "scrolling-polyphonic";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
