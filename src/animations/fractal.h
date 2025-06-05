/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class Fractal : public Animation {
   public:
    Fractal() : Animation(TransitionState::Blank, TransitionState::Blank) {
    }
    const char *name() const override {
        return "fractal";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
