/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class VerticalWipe : public Animation {
   public:
    VerticalWipe()
        : Animation(TransitionState::Anything, TransitionState::Blank) {
    }

    const char *name() const override {
        return "vertical-wipe";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};