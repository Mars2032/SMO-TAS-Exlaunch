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
    bool isRunning();
    void setScene(al::Scene* scene) { mScene = scene; };
    const char* getScriptName() { return mScriptName; };
private:
    const char* mScriptName;
    int mFrameIndex = 0;
    u32 mPrevButtons[2];
    Script* mScript;
    al::Scene* mScene;
};

