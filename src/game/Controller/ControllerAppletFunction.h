#pragma once

#include "al/gamepad/util.h"

class ControllerAppletFunction {
    public:
        static void connectControllerSinglePlay(al::GamePadSystem *);
        static void connectControllerSeparatePlay(al::GamePadSystem *);
};