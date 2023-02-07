#pragma once

#include "heap/seadDisposer.h"


struct Settings {
    SEAD_SINGLETON_DISPOSER(Settings);
    Settings();
    ~Settings();

public:
    bool isShowSensors = true;
    bool isShowEyes = true;
    bool isShowAttack = true;
    bool isShowTrans = true;
    bool isShowBindable = true;
    bool isShowNpc = true;
    bool isShowEnemyBody = true;
    bool isShowMapObj = true;
    bool isShowPlayerAll = true;
    bool isShowHoldObj = true;
    bool isShowOther = true;
    bool isShowCameraTargetTrans = true;
    float mSensorAlpha = 0.15f;
    bool isSkipCutscenes = false;
    bool isEnableAutosave = true;
    bool isEnableMoonRefresh = false;
    bool isEnableGreyMoonRefresh = false;
    s64 mRandomSeed = -1;

    //mechawiggler patterns
    struct MofumofuPatternEntry {
        const char* type;
        int target;
        bool reverse;
    };

    constexpr static const MofumofuPatternEntry mPatternEntries[23] = {
        {"Random", -1, false},
        {"Ghost", 0, false},
        {"Nose", 0, true},
        {"C", 1, false},
        {"W", 1, true},
        {"J", 2, false},
        {"Medal", 2, true},
        {"Plane", 3, false},
        {"5", 3, true},
        {"Hangman", 4, false},
        {"Spanish", 4, true},
        {"Siblings", 5, false},
        {"Snake", 5, true},
        {"8", 6, false},
        {"Mushroom", 6, true},
        {"Z", 7, false},
        {"Tetris", 7, true},
        {"Ear", 8, false},
        {"Bomb", 8, true},
        {"Bird", 9, false},
        {"L", 9, true},
        {"O", 10, false},
        {"Star", 10, true}
    };

    int mMofumofuPatternIndex = 0;
};

void drawPageMain();
void drawPageDebug();
void drawPagePrimitiveRenderer();
void drawPageAbout();
void drawPageInfo();
void drawPageTAS();
void drawPageOptions();
void drawPageGeneralOptions();
void optionsHooks();
