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

    void run(const AnimationContext& context) override;

    const char* name() const override {
        return "code-scroll";
    }

   private:
    std::vector<std::string> findFiles(const std::string& sourceDir) const;
};