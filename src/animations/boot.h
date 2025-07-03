/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <string>
#include <vector>

#include "animation.h"

// Boot animation: A special animation for system boot-up
// Based on HypnoticWaves but customized for boot sequence
class Boot : public Animation {
   public:
    Boot()
        : Animation(TransitionState::Blank, TransitionState::Blank) {
    }
    const char *name() const override {
        return "boot";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
