/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class Pyramid : public Animation {
   public:
    Pyramid() : Animation(TransitionState::Blank, TransitionState::Anything) {
    }
    const char *name() const override {
        return "pyramid";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};