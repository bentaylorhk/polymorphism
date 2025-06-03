/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <string>
#include <vector>

#include "animation.h"

// A creative animation: HypnoticWaves
// Uses trigonometric functions and intensityChars for a mesmerizing effect
class HypnoticWaves : public Animation {
   public:
    HypnoticWaves()
        : Animation(TransitionState::Blank, TransitionState::Blank) {
    }
    const char *name() const override {
        return "hypnotic-waves";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
