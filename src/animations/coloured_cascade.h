/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class ColouredCascade : public Animation {
   public:
    ColouredCascade()
        : Animation(TransitionState::Blank, TransitionState::Blank) {
    }
    const char *name() const override {
        return "coloured-cascade";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
