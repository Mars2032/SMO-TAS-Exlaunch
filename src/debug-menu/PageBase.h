#pragma once

#include "al/scene/Scene.h"
#include <gfx/seadTextWriter.h>
#include "al/scene/Scene.h"
#include "sead/basis/seadNew.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/OtherUtil.h"
#include "game/Player/PlayerActorBase.h"
#include "logger/Logger.hpp"
#include <cxxabi.h>


struct PageBase {
    PageBase(sead::Heap* heap) { mCursorPositions.allocBuffer(20, heap); } ;
    virtual void init() = 0;
    virtual void draw(al::Scene* scene, sead::TextWriter* textWriter) = 0;
    virtual void handleInput(int cursorIndex) = 0;
    void addSelectableLine(int lineNum);
    sead::PtrArray<sead::Vector2f> mCursorPositions;
    int mCursorIndex = 0;
};