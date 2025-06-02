/*
 * Benjamin Michael Taylor (bentaylorhk)
 * 2025
 */

#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <vector>

#include "code_scroll.h"
#include "dvd.h"
#include "lissajous.h"
#include "retro_cascade.h"
#include "shutterboard.h"

const std::vector<std::shared_ptr<Animation>> allAnimations = {
    // TODO: Ensure all animations done at end
    std::make_shared<Shutterboard>(), std::make_shared<CodeScroll>(),
    std::make_shared<Lissajous>(),    std::make_shared<RetroCascade>(),
    std::make_shared<DVD>(),
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