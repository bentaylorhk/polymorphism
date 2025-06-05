/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once
#include "animation.h"

class ColourPreview : public Animation {
   public:
    ColourPreview()
        : Animation(TransitionState::Blank, TransitionState::Anything) {
    }

    const char *name() const override {
        return "colour-preview";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
