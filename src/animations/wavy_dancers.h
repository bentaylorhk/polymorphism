/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class WavyDancers : public Animation {
   public:
    WavyDancers()
        : Animation(TransitionState::Blank, TransitionState::Anything) {
    }
    const char *name() const override {
        return "wavy-dancers";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
