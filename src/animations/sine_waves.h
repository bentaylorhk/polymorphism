#pragma once

#include <cmath>
#include <vector>

#include "animation.h"

class SineWaves : public Animation {
   public:
    SineWaves() : Animation(TransitionState::Blank, TransitionState::Blank) {};

    const char *name() const override {
        return "sine-waves";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
