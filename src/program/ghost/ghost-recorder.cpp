#include "ghost-recorder.h"
#include "al/sequence/Sequence.h"
#include "fs/fs_directories.hpp"
#include "game/System/GameSystem.h"
#include "helpers/fsHelper.h"
#include "result.h"

namespace {
    NERVE_DEF(GhostRecorder, Wait);
    NERVE_DEF(GhostRecorder, Record);
    NERVE_DEF(GhostRecorder, Write);
}

SEAD_SINGLETON_DISPOSER_IMPL(GhostRecorder);

GhostRecorder::GhostRecorder() : al::NerveExecutor("GhostRecorder") {
    initNerve(&nrvGhostRecorderWait, 0);
}

void setFileName() {

}

void GhostRecorder::exeWait() {

}

void GhostRecorder::exeRecord() {
    al::Scene* scene = GameSystemFunction::getGameSystem()->mSequence->mCurrentScene;
    if (!scene) return;

}

void GhostRecorder::exeWrite() {

}

