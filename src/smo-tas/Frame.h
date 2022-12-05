#pragma once

#include "math/seadVector.h"
#include "prim/seadSafeString.h"

struct Gyro {
    sead::Vector3f mAcceleration = sead::Vector3f::zero;
    sead::Vector3f mAngularVel = sead::Vector3f::zero;
    sead::Vector3f mAngle = sead::Vector3f::zero;

};

struct Frame {
    u32 mStep = 0;
    bool secondPlayer = false;
    u32 mButtons = 0;
    sead::Vector2f mLeftStick = sead::Vector2f::zero;
    sead::Vector2f mRightStick = sead::Vector2f::zero;
    sead::Vector3f mAccelLeft = sead::Vector3f::zero;
    sead::Vector3f mAccelRight = sead::Vector3f::zero;
    Gyro mGyroLeft;
    Gyro mGyroRight;
};


struct Script {
    static const u32 magic = 0x424f4f42; // BOOB
    u32 mMagic;
    char mChangeStageName[0x80];
    char mChangeStageId[0x80];
    s32 mScenarioNo;
    u32 mFrameCount;
    bool isTwoPlayer;
    Frame mFrames[];
};