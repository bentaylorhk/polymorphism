/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class CharFill : public Animation {
   public:
    CharFill() : Animation(TransitionState::Blank, TransitionState::Anything) {
    }

    const char *name() const override {
        return "char-fill";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};