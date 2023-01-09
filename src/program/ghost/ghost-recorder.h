#pragma once

#include "al/nerve/NerveExecutor.h"
#include "al/util/NerveUtil.h"
#include "fs/fs_types.hpp"

struct GhostFrame {
    sead::Vector3f mPlayerTrans;
    sead::Quatf mPlayerQuat;
    sead::FixedSafeString<128> mPlayerAnim;
    sead::FixedSafeString<128> mPlayerSubAnim;
    float mPlayerAnimFrame;
    float mPlayerSubAnimFrame;
};

//struct GhostScript {
//    bool balls;
//    GhostFrame mFrames[];
//};

class GhostRecorder : public al::NerveExecutor {
    SEAD_SINGLETON_DISPOSER(GhostRecorder);
    GhostRecorder();
    ~GhostRecorder();
public:
    void setFileName();
    void exeWait();
    void exeRecord();
    void exeWrite();
private:
    sead::FixedSafeString<128> mFileName;
    //GhostScript* mScript;
};