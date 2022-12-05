#include "smo-tas/TAS.h"
#include "al/Pad/JoyPadAccelerometerAddon.h"
#include "al/Pad/PadGyroAddon.h"
#include "al/area/ChangeStageInfo.h"
#include "al/util/NerveUtil.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "fs/fs_files.hpp"
#include "game/Controller/ControllerAppletFunction.h"
#include "game/System/GameSystem.h"
#include "rs/util/LiveActorUtil.h"
#include "sead/basis/seadNew.h"

namespace {
    NERVE_DEF(TAS, Update);
    NERVE_DEF(TAS, Wait);
}

SEAD_SINGLETON_DISPOSER_IMPL(TAS);

TAS::TAS() : al::NerveExecutor("TAS") {
    initNerve(&nrvTASWait, 0);
}

TAS::~TAS() = default;

bool TAS::tryLoadScript(nn::fs::DirectoryEntry& entry) {
    endScript();
    mScriptName = entry.m_Name;
    sead::FormatFixedSafeString<256> scriptPath("sd:/scripts/%s", entry.m_Name);
    nn::fs::FileHandle handle;
    nn::Result r = nn::fs::OpenFile(&handle,scriptPath.cstr(),nn::fs::OpenMode::OpenMode_Read);
    if (R_FAILED(r)) return false;
    mScript = (Script*)new (al::getSequenceHeap())u8[entry.m_FileSize];
    r = nn::fs::ReadFile(handle, 0,mScript,entry.m_FileSize);
    nn::fs::CloseFile(handle);
    if (R_FAILED(r)) {
        endScript();
        return false;
    }
    if (mScript->mMagic != Script::magic) {
        endScript();
        return false;
    }
    return true;
}

void TAS::startScript() {
    GameDataHolderAccessor accessor(mScene);
    if (mScript->isTwoPlayer != rs::isSeparatePlay(mScene)) {
        al::GamePadSystem* gamePadSystem = GameSystemFunction::getGameSystem()->mGamePadSystem;
        if (mScript->isTwoPlayer)
            ControllerAppletFunction::connectControllerSeparatePlay(gamePadSystem);
        else
            ControllerAppletFunction::connectControllerSinglePlay(gamePadSystem);
    }

    if (!al::isEqualString(mScript->mChangeStageName,"")) {
        ChangeStageInfo info(accessor.mData, mScript->mChangeStageId,
                             mScript->mChangeStageName, false, mScript->mScenarioNo, ChangeStageInfo::UNK);
        accessor.mData->changeNextStage(&info, 0);
    }

    al::setNerve(this, &nrvTASUpdate);
}

void TAS::endScript() {
    al::setNerve(this, &nrvTASWait);
    operator delete (mScript, al::getSequenceHeap());
    mScriptName = "";
}

void TAS::applyFrame(Frame& frame) {
    sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
    sead::Controller* controller = controllerMgr->getController(al::getPlayerControllerPort(frame.secondPlayer));
    auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAccel, 0);
    auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAccel, 1);
    auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cGyro, 0);
    auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cGyro, 1);
    // do all the controller shit with frame
    controller->mLeftStick = frame.mLeftStick;
    controller->mRightStick = frame.mRightStick;
    accelLeft->mAcceleration = frame.mAccelLeft;
    accelRight->mAcceleration = frame.mAccelRight;
    gyroLeft->mAcceleration = frame.mGyroLeft.mAcceleration;
    gyroLeft->mAngularVel = frame.mGyroLeft.mAngularVel;
    gyroLeft->mAngle = frame.mGyroLeft.mAngle;
    gyroRight->mAcceleration = frame.mGyroRight.mAcceleration;
    gyroRight->mAngularVel = frame.mGyroRight.mAngularVel;
    gyroRight->mAngle = frame.mGyroRight.mAngle;

    controller->mPadTrig = frame.mButtons & ~mPrevButtons[frame.secondPlayer];
    controller->mPadRelease = frame.mButtons & mPrevButtons[frame.secondPlayer];
    controller->mPadHold = frame.mButtons;
}

void TAS::exeUpdate() {
    if (al::isFirstStep(this)) {
        mFrameIndex = 0;
        mPrevButtons[0] = 0;
        mPrevButtons[1] = 0;
    }
    int step = al::getNerveStep(this);

    bool updated[2] = { false, false };

    while (mFrameIndex < mScript->mFrameCount) {
        Logger::log("Frame Index: %d\n", mFrameIndex);
        Frame& curFrame = mScript->mFrames[mFrameIndex];
        if (step < curFrame.mStep) break;
        mFrameIndex++; // increment after checking step
        updated[curFrame.secondPlayer] = true;
        applyFrame(curFrame);

        mPrevButtons[curFrame.secondPlayer] = curFrame.mButtons;
    }

    if (!updated[0]) {
        mPrevButtons[0] = 0;
        Frame frame = {.secondPlayer = false};
        applyFrame(frame);
    }
    if (!updated[1]) {
        mPrevButtons[1] = 0;
        Frame frame = {.secondPlayer = true};
        applyFrame(frame);
    }
    if ((mFrameIndex + 1) >= mScript->mFrameCount) al::setNerve(this, &nrvTASWait);

}

void TAS::exeWait() {

}

bool TAS::isRunning() {
    return al::isNerve(this, &nrvTASUpdate);
}