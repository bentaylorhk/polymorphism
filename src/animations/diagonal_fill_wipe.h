/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class DiagonalFillWipe : public Animation {
   public:
    DiagonalFillWipe()
        : Animation(TransitionState::Anything, TransitionState::Blank) {
    }

    const char *name() const override {
        return "diagonal-fill-wipe";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};