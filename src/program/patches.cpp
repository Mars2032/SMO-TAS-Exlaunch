#include "patches.hpp"
#include "al/util/RandomUtil.h"
#include "debug-menu/ImGuiMenu.h"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;


//mechawiggler pattern setting
bool isPatternReverse() {
    auto* settings = Settings::instance();
    if (settings->mPatternEntries[settings->mMofumofuPatternIndex].target == -1) { //if random pattern
        return al::isHalfProbability();
    }
    else {
        return settings->mPatternEntries[settings->mMofumofuPatternIndex].reverse;
    }
}

int getMofumofuTarget(int a) {
    auto* settings = Settings::instance();
    if (settings->mPatternEntries[settings->mMofumofuPatternIndex].target == -1) { //if random pattern
        return al::getRandom(a);
    }
    else {
        return settings->mPatternEntries[settings->mMofumofuPatternIndex].target;
    }
}

void mechawigglerPatches() {
    patch::CodePatcher(0x0b07a8).BranchLinkInst((void*) isPatternReverse);
    patch::CodePatcher(0x0b07f8).BranchLinkInst((void*) getMofumofuTarget);
}

void stubSocketInit() {
    patch::CodePatcher p(0x95C498);
    p.WriteInst(inst::Nop());
}

void runCodePatches() {
    mechawigglerPatches();
    stubSocketInit();
}