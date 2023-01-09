#pragma once

#include "fs_types.hpp"
#include "result.h"

namespace nn::fs {

    /*
        Mount SD card. Must have explicit permission.
        mount: drive to mount to.
    */
    nn::Result MountSdCardForDebug(char const* mount);
};