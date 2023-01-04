#include "patches.hpp"
#include "debug-menu/Menu.h"
#include "al/util/RandomUtil.h"

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

void costumeRoomPatches() {
    patch::CodePatcher p(0x262850);
    p.WriteInst(inst::Movz(reg::W0, 0));
    p.Seek(0x2609B4);
    p.WriteInst(inst::Movz(reg::W0, 0));

    p.Seek(0x25FF74);
    p.WriteInst(inst::Movz(reg::W0, 1));
    p.Seek(0x25FF74);
    p.WriteInst(inst::Movz(reg::W0, 0));
}

//mechawiggler pattern setting
bool isPatternReverse() {
    Menu* menu = Menu::instance();
    if (menu->mPatternEntries[menu->mofumofuPatternIndex].target == -1) { //if random pattern
        return al::isHalfProbability();
    }
    else {
        return menu->mPatternEntries[menu->mofumofuPatternIndex].reverse;
    }
}

int getMofumofuTarget(int a) {
    Menu* menu = Menu::instance();
    if (menu->mPatternEntries[menu->mofumofuPatternIndex].target == -1) { //if random pattern
        return al::getRandom(a);
    }
    else {
        return menu->mPatternEntries[menu->mofumofuPatternIndex].target;
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
    costumeRoomPatches();
    mechawigglerPatches();
    stubSocketInit();
}