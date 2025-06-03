/**
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include "animation.h"

class Words : public Animation {
   public:
    Words() : Animation(TransitionState::Blank, TransitionState::Blank) {
    }

    const char *name() const override {
        return "words";
    }

   protected:
    void drawFrame(const AnimationContext &context) override;

   private:
    std::vector<std::string> words = {"Polyphonic", "RSVP",
                                      "is",         "a",
                                      "collection", "of",
                                      "ASCII",      "art",
                                      "animations", "made",
                                      "for",        "the",
                                      "event",      "Polyphonic RSVP",
                                      "by",         "Benjamin Michael Taylor"};
};