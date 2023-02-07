#pragma once

#include "../nerve/NerveExecutor.h"
#include "al/nerve/NerveExecutor.h"
namespace al
{
    class CameraVerticalAbsorber : public al::NerveExecutor {
        public:
            bool isValid(void);
            
    };
};