#pragma once

#include "debug-menu/PageBase.h"

struct PageMain : public PageBase {
    PageMain(sead::Heap* heap) : PageBase(heap) {};
    void init() override;
    void handleInput(int cursorIndex) override;
    void draw(al::Scene* scene, sead::TextWriter* textWriter) override;
};