#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotImplementedFunctions"
#pragma once

#include "math/seadVector.h"
#include "sead/controller/seadControllerAddon.h"

namespace al {
class PadGyroAddon : public sead::ControllerAddon {
    SEAD_RTTI_OVERRIDE(PadGyroAddon, sead::ControllerAddon);
    PadGyroAddon(sead::Controller*, int);
    ~PadGyroAddon() override;
public:
    bool calc() override;
    bool tryUpdateGyroStatus();
    void getPose(sead::Vector3f* roll, sead::Vector3f* pitch, sead::Vector3f* yaw);
    void setPose(sead::Vector3f const& roll, sead::Vector3f const& pitch, sead::Vector3f const& yaw)
                {
        mAcceleration = roll;
        mAngularVel = pitch;
        mAngle = yaw; };
public:
    char _padding[0x4];
    sead::Vector3f mAcceleration;
    sead::Vector3f mAngularVel;
    sead::Vector3f mAngle;
};

}

#pragma clang diagnostic pop