/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class GameOfLife : public Animation {
   public:
    GameOfLife() : Animation(TransitionState::Logo, TransitionState::Anything) {
    }

    const char *name() const override {
        return "game-of-life";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
};
