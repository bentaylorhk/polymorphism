/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once
#include <random>

#include "animation.h"

class DVD : public Animation {
   public:
    DVD() : Animation(TransitionState::Blank, TransitionState::Blank) {
    }
    const char *name() const override {
        return "dvd";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
