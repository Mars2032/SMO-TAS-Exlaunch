#pragma once

#include "agl/DrawContext.h"
#include "agl/RenderBuffer.h"

namespace al {
    class GameDrawInfo {
    public:
        agl::RenderBuffer* getRenderBuffer() {
            return mIsDocked ? mDockedRenderBuffer : mHandheldRenderBuffer;
        }
        agl::RenderBuffer* mDockedRenderBuffer;
        agl::RenderBuffer* mHandheldRenderBuffer;
        bool mIsDocked;
        agl::DrawContext* mDrawContext;
    };
}
