/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once
#include "animation.h"

class RetroCascade : public Animation {
   public:
    RetroCascade() : Animation(TransitionState::Blank, TransitionState::Blank) {
    }
    const char *name() const override {
        return "retro-cascade";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
