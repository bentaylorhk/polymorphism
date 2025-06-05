/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class Starfield : public Animation {
   public:
    Starfield() : Animation(TransitionState::Blank, TransitionState::Anything) {
    }

    const char *name() const override {
        return "starfield";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};