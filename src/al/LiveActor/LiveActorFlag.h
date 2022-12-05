#pragma once

namespace al {
    struct LiveActorFlag {
        bool mDead;
        bool mClipped;
        bool mClippingInvalidated;
        bool mDrawClipped;
        bool mCalcAnimOn;
        bool mModelVisible;
        bool mNoCollide;
        bool mUnknown;
        bool mMaterialCodeValid;
        bool mAreaTarget;
        bool mUpdateMovementEffectAudioCollisionSensor;
    };
}