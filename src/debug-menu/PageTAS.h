#pragma once

#include "debug-menu/PageBase.h"
#include "fs/fs_types.hpp"

struct PageTAS : public PageBase {
    PageTAS(sead::Heap* heap) : PageBase(heap) {};
    void init() override;
    void handleInput(int cursorIndex) override;
    void draw(al::Scene* scene, sead::TextWriter* textWriter) override;
    void updateDir();
    nn::fs::DirectoryEntry* mEntries = nullptr;
    s64 mEntryCount;
};
