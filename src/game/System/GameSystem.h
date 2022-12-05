#pragma once

#include "GameSystemInfo.h"
#include "al/nerve/NerveExecutor.h"

namespace al {
    class AudioResourceLoadInfo;
    class NfpDirector;
    class ApplicationMessageReceiver;
    class Sequence;
    class AccountHolder;
} // namespace al

class GameConfigData;

class GameSystem : public al::NerveExecutor {
public:
    al::Sequence* mSequence;
    al::GameSystemInfo* mGameSystemInfo;
    al::AudioSystem* mAudioSystem;
    sead::PtrArray<al::AudioResourceLoadInfo> mAudioResourceLoadInfo;
    al::AccountHolder* mAccountHolder;
    al::NetworkSystem* mNetworkSystem;
    char unk[8];
    al::HtmlViewer* mHtmlViewer;
    al::NfpDirector* mNfpDirector;
    al::GamePadSystem* mGamePadSystem;
    al::ApplicationMessageReceiver* mApplicationMessageReceiver;
    al::WaveVibrationHolder* mWaveVibrationHolder;
    bool unk1;
    GameConfigData* mGameConfigData;
    bool unk2;
};

namespace GameSystemFunction {
    GameSystem* getGameSystem();
}