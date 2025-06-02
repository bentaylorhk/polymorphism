/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class FuzzOut : public Animation {
   public:
    FuzzOut() : Animation(TransitionState::Anything, TransitionState::Blank) {
    }

    const char *name() const override {
        return "fuzz-out";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};