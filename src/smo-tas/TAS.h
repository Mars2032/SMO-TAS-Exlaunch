#pragma once

#include "debug-menu/Menu.h"
#include "smo-tas/Frame.h"

class TAS : public al::NerveExecutor {
    SEAD_SINGLETON_DISPOSER(TAS);
    TAS();
    ~TAS();
public:
    bool tryLoadScript(nn::fs::DirectoryEntry& entry);
    void startScript();
    void endScript();
    void applyFrame(Frame& frame);
    void exeUpdate();
    void exeWait();
    void exeWaitUpdate();
    void exeRecord();
    bool isRunning();
    void setScene(al::Scene* scene) { mScene = scene; };
    al::Scene* getScene() const { return mScene; };
    const char* getScriptName() const { return mScriptName.cstr(); };
private:
    sead::FixedSafeString<0x80> mScriptName;
    int mFrameIndex = 0;
    u32 mPrevButtons[2];
    Script* mScript;
    al::Scene* mScene;
};

