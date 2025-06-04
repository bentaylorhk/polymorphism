/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once
#include "animation.h"

class Shutterboard : public Animation {
   public:
    Shutterboard()
        : Animation(TransitionState::Blank, TransitionState::Anything) {
    }

    const char* name() const override {
        return "shutterboard";
    }

   protected:
    void drawFrame(const AnimationContext& context) override;
};