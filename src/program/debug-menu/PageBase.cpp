#include "debug-menu/PageBase.h"

void PageBase::addSelectableLine(int lineNum) {
    sead::Vector2f* cursorPos = new sead::Vector2f(10.f, 270.f+ 20.f*(float)lineNum);
    mCursorPositions.pushBack(cursorPos);
}