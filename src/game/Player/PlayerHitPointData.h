#pragma once

#include "al/byaml/ByamlSave.h"
#include "types.h"

class PlayerHitPointData : public ByamlSave {
public:
    PlayerHitPointData();
    void write(al::ByamlWriter *) override;
    void read(al::ByamlIter const&) override;

    void setKidsModeFlag(bool);
    void init();
    void recoverMax();
    int getCurrent() const;
    int getMaxCurrent() const;
    int getMaxWithItem() const;
    int getMaxWithoutItem() const;
    bool isMaxCurrent() const;
    bool isMaxWithItem() const;
    void getMaxUpItem();
    void recover();
    void recoverForDebug();
    void damage();
    void kill();
    void forceNormalMode();
    void endForceNormalMode();
    bool isForceNormalMode() const;

    bool mIsKidsMode; // 0x8
    int mCurrentHit; // 0xC
    bool mIsHaveMaxUpItem; // 0x10
    bool mIsForceNormalHealth; // 0x11
};