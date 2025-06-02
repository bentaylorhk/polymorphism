/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once
#include "animation.h"

constexpr int MAX_CURVES = 3;

class Lissajous : public Animation {
   public:
    Lissajous() : Animation(TransitionState::Blank, TransitionState::Anything) {
    }

    const char* name() const override {
        return "lissajous";
    }

   protected:
    void drawFrame(const AnimationContext& context) override;
};