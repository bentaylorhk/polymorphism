/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class RenaeDancers : public Animation {
   public:
    RenaeDancers()
        : Animation(TransitionState::Blank, TransitionState::Anything) {
    }
    const char *name() const override {
        return "renae-dancers";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
