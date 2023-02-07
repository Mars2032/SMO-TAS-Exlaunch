#pragma once

#include "CameraPoser.h"

class CameraPoserFollowLimit : public al::CameraPoser {
    public:
        unsigned char massive[0x200];

        //sead::Vector3f lookAtPos; // 0x1F4
};