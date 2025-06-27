/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class Neofetch : public Animation {
   public:
    Neofetch();
    const char *name() const override {
        return "neofetch";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;
    std::vector<std::string> infoLines;
};
