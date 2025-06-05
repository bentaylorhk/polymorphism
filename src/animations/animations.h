/*
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once

#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "../util/common.h"
#include "char_fill.h"
#include "code_scroll.h"
#include "coloured_cascade.h"
#include "diagonal_fill_wipe.h"
#include "dvd.h"
#include "fuzz_out.h"
// #include "game_of_life.h"
#include "fractal.h"
#include "hypnotic_waves.h"
#include "line_wipe.h"
#include "lissajous.h"
#include "moving_wipe.h"
#include "neofetch.h"
// #include "shutterboard.h"
#include "colour_preview.h"
#include "pyramid.h"
#include "sine_waves.h"
#include "single_cascade.h"
#include "verbs.h"
#include "vertical_wipe.h"

const std::vector<std::shared_ptr<Animation>> allAnimations = {
    // TODO: Ensure all animations done at end
    // std::make_shared<Shutterboard>(),
    std::make_shared<CodeScroll>(), std::make_shared<Lissajous>(),
    std::make_shared<SingleCascade>(), std::make_shared<DVD>(),
    // TODO: game of life
    // std::make_shared<GameOfLife>(),
    std::make_shared<CharFill>(), std::make_shared<DiagonalFillWipe>(),
    std::make_shared<SineWaves>(), std::make_shared<FuzzOut>(),
    std::make_shared<VerticalWipe>(), std::make_shared<HypnoticWaves>(),
    std::make_shared<ColouredCascade>(), std::make_shared<LineWipe>(),
    std::make_shared<MovingWipe>(), std::make_shared<Verbs>(),
    std::make_shared<ColourPreview>(), std::make_shared<Fractal>(),
    std::make_shared<Pyramid>(),

    // TODO: Finish neofetch, could make a full one in house and display ncurses
    // colours used.
    // std::make_shared<Neofetch>(),
};

std::map<TransitionState, std::vector<std::shared_ptr<Animation>>>
getAnimationsByStartState() {
    std::map<TransitionState, std::vector<std::shared_ptr<Animation>>> stateMap;
    for (const auto& animation : allAnimations) {
        stateMap[animation->startState].push_back(animation);
    }
    return stateMap;
}

std::shared_ptr<Animation> findAnimationByName(const std::string& name) {
    auto it =
        std::find_if(allAnimations.begin(), allAnimations.end(),
                     [&name](const std::shared_ptr<Animation>& animation) {
                         return animation->name() == name;
                     });
    if (it != allAnimations.end()) {
        return *it;
    }
    return nullptr;
}