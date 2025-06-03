/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class LineWipe : public Animation {
   public:
    LineWipe() : Animation(TransitionState::Anything, TransitionState::Blank) {
    }

    const char *name() const override {
        return "line-wipe";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
