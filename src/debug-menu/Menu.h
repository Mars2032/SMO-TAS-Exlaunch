#pragma once

#include "debug-menu/PageAbout.h"
#include "debug-menu/PageBase.h"
#include "debug-menu/PageHitSensor.h"
#include "debug-menu/PageInfo.h"
#include "debug-menu/PageMain.h"
#include "debug-menu/PageTAS.h"
#include "debug-menu/PageOptions.h"

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
    PageBase* mCurPage = nullptr;
    int mCursorIndex = 0;
    float mSensorAlpha = 0.15;
    sead::TextWriter* mTextWriter = nullptr;
};