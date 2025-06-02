/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class SingleCascade : public Animation {
   public:
    SingleCascade()
        : Animation(TransitionState::Blank, TransitionState::Blank) {
    }
    const char *name() const override {
        return "single-cascade";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
