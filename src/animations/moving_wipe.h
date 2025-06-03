/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class MovingWipe : public Animation {
   public:
    MovingWipe()
        : Animation(TransitionState::Anything, TransitionState::Blank) {
    }
    const char *name() const override {
        return "moving-wipe";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
