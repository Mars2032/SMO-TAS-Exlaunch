#include "smo-tas/TAS.h"
#include "al/Pad/JoyPadAccelerometerAddon.h"
#include "al/Pad/PadGyroAddon.h"
#include "al/area/ChangeStageInfo.h"
#include "al/util/NerveUtil.h"
#include "al/util.hpp"
#include "filedevice/seadFileDeviceMgr.h"
#include "fs/fs_files.hpp"
#include "fs/fs_directories.hpp"
#include "game/Controller/ControllerAppletFunction.h"
#include "game/System/GameSystem.h"
#include "rs/util/LiveActorUtil.h"
#include "sead/basis/seadNew.h"
#include "al/Pad/NpadController.h"

namespace {
    NERVE_DEF(TAS, Update);
    NERVE_DEF(TAS, Wait);
    NERVE_DEF(TAS, WaitUpdate);
    NERVE_DEF(TAS, Record);
}

SEAD_SINGLETON_DISPOSER_IMPL(TAS);

TAS::TAS() : al::NerveExecutor("TAS") {
    initNerve(&nrvTASWait, 0);
}

TAS::~TAS() = default;

void TAS::updateDir() {
    nn::fs::DirectoryHandle handle;
    nn::Result r = nn::fs::OpenDirectory(&handle, "sd:/scripts", nn::fs::OpenDirectoryMode_File);
    if (R_FAILED(r)) return;
    s64 entryCount;
    r = nn::fs::GetDirectoryEntryCount(&entryCount, handle);
    if (R_FAILED(r)) {
        nn::fs::CloseDirectory(handle);
        return;
    }
    nn::fs::DirectoryEntry* entryBuffer = new nn::fs::DirectoryEntry[entryCount];
    r = nn::fs::ReadDirectory(&entryCount, entryBuffer, handle, entryCount);
    nn::fs::CloseDirectory(handle);
    if (R_FAILED(r)) {
        delete[] entryBuffer;
        return;
    }
    delete[] mEntries;
    mEntries = entryBuffer;
    mEntryCount = entryCount;
}

bool TAS::refreshCurrentScriptEntry() { //returns false if no file exists with the current script name
    oldScriptEntry = currentScriptEntry;
    updateDir();
    for (int i = 0; i < mEntryCount; i++) {
        if (strcmp(oldScriptEntry.m_Name, mEntries[i].m_Name) == 0) {
            currentScriptEntry = mEntries[i];
            return true;
        }
    }
    return false;
};

//tries to start the current script
bool TAS::tryStartScript() {
    if (tryLoadScript()) {
        startScript();
        return true;
    }
    return false;
}

bool TAS::tryLoadScript() {
    endScript();
    if (!refreshCurrentScriptEntry()) {
        return false;
    }
    mScriptName = currentScriptEntry.m_Name;
    sead::FormatFixedSafeString<256> scriptPath("sd:/scripts/%s", currentScriptEntry.m_Name);
    nn::fs::FileHandle handle;
    nn::Result r = nn::fs::OpenFile(&handle,scriptPath.cstr(),nn::fs::OpenMode::OpenMode_Read);
    if (R_FAILED(r)) return false;
    mScript = (Script*)new (al::getSequenceHeap())u8[currentScriptEntry.m_FileSize];
    r = nn::fs::ReadFile(handle, 0,mScript,currentScriptEntry.m_FileSize);
    nn::fs::CloseFile(handle);
    if (R_FAILED(r)) {
        endScript();
        mScriptName = "";
        return false;
    }
    if (mScript->mMagic != Script::magic) {
        endScript();
        mScriptName = "";
        return false;
    }
    return true;
}

void TAS::startScript() {
    Logger::log("In startScript()\n");
    bool isWait = false;
    if (mScript->isTwoPlayer != rs::isSeparatePlay(mScene)) {
        al::GamePadSystem* gamePadSystem = GameSystemFunction::getGameSystem()->mGamePadSystem;
        if (mScript->isTwoPlayer) {
            if (!ControllerAppletFunction::connectControllerSeparatePlay(gamePadSystem)) return;
            rs::changeSeparatePlayMode(mScene, true);
            isWait = true;
        } else {
            if (!ControllerAppletFunction::connectControllerSinglePlay(gamePadSystem)) return;
            rs::changeSeparatePlayMode(mScene, false);
            isWait = true;
        }
    }
    Logger::log("In Correct Mode\n");

    GameDataHolderAccessor accessor(mScene);
    if (!al::isEqualString(mScript->mChangeStageName,"")) {
        ChangeStageInfo info(accessor.mData, mScript->mChangeStageId,
                             mScript->mChangeStageName, false, mScript->mScenarioNo, ChangeStageInfo::UNK);
        accessor.mData->changeNextStage(&info, 0);
        isWait = true;
    }
    Logger::log("Starting Script: %s\n", mScriptName.cstr());
    if (isWait)
        al::setNerve(this, &nrvTASWaitUpdate);
    else
        al::setNerve(this, &nrvTASUpdate);
}

void TAS::endScript() {
    al::setNerve(this, &nrvTASWait);
    operator delete (mScript, al::getSequenceHeap());
    //mScriptName = "";
}

void TAS::applyFrame(Frame& frame) {
    sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
    auto* controller = (al::NpadController*)controllerMgr->getController(al::getPlayerControllerPort(frame.secondPlayer));
    controller->mPadAccelerationDeviceNum = 2; // number of accelerometers for joycons
    auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAccel, 0);
    auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAccel, 1);
    auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cGyro, 0);
    auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cGyro, 1);
    // do all the controller shit with frame
    controller->mLeftStick = frame.mLeftStick;
    controller->mRightStick = frame.mRightStick;
    accelLeft->mAcceleration = frame.mLeftAccel;
    accelRight->mAcceleration = frame.mRightAccel;
    gyroLeft->mDirection = frame.mLeftGyro.mDirection;
    gyroRight->mDirection = frame.mRightGyro.mDirection;
    gyroLeft->mAngularVel = frame.mLeftGyro.mAngularV;
    gyroRight->mAngularVel = frame.mRightGyro.mAngularV;

    controller->mPadTrig = frame.mButtons & ~mPrevButtons[frame.secondPlayer];
    controller->mPadRelease = frame.mButtons & mPrevButtons[frame.secondPlayer];
    controller->mPadHold = frame.mButtons;
}

void TAS::exeUpdate() {
    if (al::isFirstStep(this)) {
        Logger::log("Update First Step\n");
        mFrameIndex = 0;
        mPrevButtons[0] = 0;
        mPrevButtons[1] = 0;
        PlayerActorBase* playerBase = rs::getPlayerActor(mScene);
        if (playerBase && !(mScript->mStartPosition.x == 0 && mScript->mStartPosition.y == 0 && mScript->mStartPosition.z == 0)) { //teleport unless position is (0, 0, 0)
            playerBase->startDemoPuppetable();
            al::setTrans(playerBase, mScript->mStartPosition);
            playerBase->endDemoPuppetable();
        }
    }
    int step = al::getNerveStep(this);

    bool updated[2] = { false, false };

    while (mFrameIndex < mScript->mFrameCount) {
        Logger::log("Frame Index: %d, Step: %d\n", mFrameIndex, al::getNerveStep(this));
        Frame& curFrame = mScript->mFrames[mFrameIndex];
        if (step < curFrame.mStep) break;
        mFrameIndex++; // increment after checking step
        updated[curFrame.secondPlayer] = true;
        applyFrame(curFrame);

        mPrevButtons[curFrame.secondPlayer] = curFrame.mButtons;
        Logger::log("Applied frame: %d, step: %d\n", mFrameIndex, curFrame.mStep);
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
    if (mFrameIndex >= mScript->mFrameCount) {
        Logger::log("Ended Script on Step: %d\n", al::getNerveStep(this));
        //al::setNerve(this, &nrvTASWait);
        endScript();
    }

}

int TAS::getFrameIndex() {
    return mFrameIndex;
}

int TAS::getFrameCount() {
    return mScript->mFrameCount;
}

sead::Vector3f TAS::getStartPosition() {
    return mScript->mStartPosition;
}

bool TAS::hasScript() {
    return (mScript);
}

void TAS::exeWait() {

}

void TAS::exeWaitUpdate() {
    al::setNerve(this, &nrvTASUpdate);
}

void TAS::exeRecord() {

}

bool TAS::isRunning() {
    return al::isNerve(this, &nrvTASUpdate);
}