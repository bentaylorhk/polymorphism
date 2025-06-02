/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once
#include "animation.h"

class CodeScroll : public Animation {
   public:
    CodeScroll()
        : Animation(TransitionState::Blank, TransitionState::Anything) {
    }

    const char* name() const override {
        return "code-scroll";
    }

   protected:
    void drawFrame(const AnimationContext& context) override;

   private:
    std::vector<std::string> findFiles(const std::string& sourceDir) const;
};