/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class Verbs : public Animation {
   public:
    Verbs() : Animation(TransitionState::Blank, TransitionState::Blank) {
    }

    const char *name() const override {
        return "verbs";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;

   private:
    std::vector<std::string> verbs = {
        "live",    "laugh",    "breathe",  "dance",   "cry",   "feel",
        "forget",  "remember", "heal",     "love",    "grow",  "change",
        "embrace", "release",  "discover", "explore", "shine", "forgive",
        "belong",  "wander",   "trust",    "doubt",   "hope",  "listen",
        "dream",   "fight",    "create",   "move",    "risk",  "accept",
        "search",  "endure",   "stumble"};
};