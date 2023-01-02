#pragma once

#include "debug-menu/PageAbout.h"
#include "debug-menu/PageBase.h"
#include "debug-menu/PageHitSensor.h"
#include "debug-menu/PageInfo.h"
#include "debug-menu/PageMain.h"
#include "debug-menu/PageTAS.h"
#include "debug-menu/PageOptions.h"
#include "debug-menu/PageDebug.h"

struct Menu {
    SEAD_SINGLETON_DISPOSER(Menu);
    Menu();
    ~Menu();
public:
    void init(sead::Heap* heap, sead::TextWriter* textWriter);
    void setCurPage(PageBase* page);
    void draw(al::Scene* scene);
    void handleInput();
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
    bool isHandleInputs = true;
    PageMain* mPageMain = nullptr;
    PageTAS* mPageTAS = nullptr;
    PageInfo* mPageInfo = nullptr;
    PageAbout* mPageAbout = nullptr;
    PageHitSensor* mPageHitSensor = nullptr;
    PageOptions* mPageOptions = nullptr;
    PageDebug* mPageDebug = nullptr;
    PageBase* mCurPage = nullptr;
    int mCursorIndex = 0;
    float mSensorAlpha = 0.15;
    sead::TextWriter* mTextWriter = nullptr;

    bool isTriggerUp() const { return mCurPadInputs & 1; }
    bool isTriggerDown() const { return mCurPadInputs & 2; }
    bool isTriggerLeft() const { return mCurPadInputs & 4; }
    bool isTriggerRight() const { return mCurPadInputs & 8; }

    //mechawiggler patterns
    struct MofumofuPatternEntry {
        const char* type;
        int target;
        bool reverse;
    };

    constexpr static const MofumofuPatternEntry mPatternEntries[23] = {
        {"Random", -1, false}, {"Ghost", 0, false}, {"Nose", 0, true}, {"C", 1, false}, {"W", 1, true}, {"J", 2, false}, {"Medal", 2, true}, {"Plane", 3, false}, {"5", 3, true}, {"Hangman", 4, false}, {"Spanish", 4, true},
        {"Siblings", 5, false}, {"Snake", 5, true}, {"8", 6, false}, {"Mushroom", 6, true}, {"Z", 7, false}, {"Tetris", 7, true}, {"Ear", 8, false}, {"Bomb", 8, true}, {"Bird", 9, false}, {"L", 9, true}, {"O", 10, false}, {"Star", 10, true}
    };

    int mofumofuPatternIndex = 0;

private:
    u8 mPrevPadInputs = 0;
    u8 mCurPadInputs = 0;
};