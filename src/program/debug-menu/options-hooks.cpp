#include "ImGuiMenu.h"
#include "al/LiveActor/LiveActor.h"
#include "game/StageScene/StageScene.h"
#include "logger/Logger.hpp"
#include "os/os_tick.hpp"
#include "patch/code_patcher.hpp"
#include "random/seadGlobalRandom.h"

class DemoStateHackFirst;

namespace patch = exl::patch;
namespace inst = exl::armv8::inst;
namespace reg = exl::armv8::reg;

HOOK_DEFINE_TRAMPOLINE(StateHackDemo) {
    static bool Callback(DemoStateHackFirst* thisPtr) {
        //Logger::log("State Hack Demo Hook\n");
        auto* settings = Settings::instance();
    if (settings->isSkipCutscenes)
        return true;
    else
        return Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(BossBattleDemo) {
    static bool Callback(al::LiveActor const* a, int b, int c) {
        //Logger::log("Boss Battle Hook\n");
        auto* settings = Settings::instance();
        if (settings->isSkipCutscenes)
            return false;
        else
            return Orig(a, b, c);
    }
};

HOOK_DEFINE_TRAMPOLINE(SaveHook) {
    static bool Callback(StageScene* scene) {
        auto* settings = Settings::instance();
        if (settings->isEnableAutosave)
            return Orig(scene);
        else
            return false;
    }
};

HOOK_DEFINE_TRAMPOLINE(ShineRefreshHook) {
    static void Callback(GameDataHolderWriter writer, ShineInfo const* shineInfo) {
        auto* settings = Settings::instance();
        if (!settings->isEnableMoonRefresh)
            Orig(writer, shineInfo);
    }
};

HOOK_DEFINE_TRAMPOLINE(GreyShineRefreshHook) {
    static bool Callback(GameDataHolderWriter writer, ShineInfo const* shineInfo) {
        auto* settings = Settings::instance();
        if (settings->isEnableGreyMoonRefresh)
            return false;
        return Orig(writer, shineInfo);
    }
};

static int calls = 0;

HOOK_DEFINE_TRAMPOLINE(GetRandomHook) {
    static u32 Callback(sead::Random* random) {
//        Logger::log("sead::Random::getU32 has been called in thread: %s\n", name);
        if (random == sead::GlobalRandom::instance()) {
            uintptr_t ptrToX30;
            __asm ("MOV %[result], X30" : [result] "=r" (ptrToX30));
            uintptr_t start = exl::util::GetMainModuleInfo().m_Total.m_Start;
            const char* name = nn::os::GetCurrentThread()->thread_name_addr;
            Logger::log("GlobalRandom mX: %x\tmY: %x\tmZ: %x\tmW: %x\tThread: %s\tAddress: %p\n", random->mX, random->mY, random->mZ, random->mW, name, ptrToX30 - start);
            calls++;
        }
        return Orig(random);
    }
};

HOOK_DEFINE_TRAMPOLINE(RandomInitHook) {
    static void Callback(sead::Random* random, u32 seed) {
        auto* settings = Settings::instance();
        if (random == sead::GlobalRandom::instance())
            Logger::log("Settings Value: %d\n", settings->mRandomSeed);
        if (settings->mRandomSeed == -1)
            Orig(random, seed);
        else
            Orig(random, settings->mRandomSeed);
    }
};

s64 randomHook(sead::Random* random) {
    auto* settings = Settings::instance();
    if (random == sead::GlobalRandom::instance())
        Logger::log("Settings Value: %d\n", settings->mRandomSeed);
    if (settings->mRandomSeed == -1)
        return nn::os::GetSystemTick().GetInt64Value();
    return settings->mRandomSeed;
}


void optionsHooks() {
    //Logger::log("optionsHooks has run.\n");
    StateHackDemo::InstallAtSymbol("_ZNK18DemoStateHackFirst20isEnableShowHackDemoEv");
    BossBattleDemo::InstallAtSymbol("_ZN2rs32isAlreadyShowDemoBossBattleStartEPKN2al9LiveActorEii");
    SaveHook::InstallAtSymbol("_ZNK10StageScene12isEnableSaveEv");
    ShineRefreshHook::InstallAtSymbol("_ZN16GameDataFunction11setGotShineE20GameDataHolderWriterPK9ShineInfo");
    GreyShineRefreshHook::InstallAtSymbol("_ZN16GameDataFunction10isGotShineE22GameDataHolderAccessorPK9ShineInfo");
    patch::CodePatcher p(0x762020);
    //p.BranchLinkInst((void*)&randomHook);
    //GetRandomHook::InstallAtSymbol("_ZN4sead6Random6getU32Ev");
    //RandomInitHook::InstallAtSymbol("_ZN4sead6Random4initEj");
}