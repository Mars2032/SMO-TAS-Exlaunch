#include "debug-menu/Menu.h"

SEAD_SINGLETON_DISPOSER_IMPL(Menu);

Menu::Menu() = default;

Menu::~Menu() = default;

void Menu::init(sead::Heap* heap, sead::TextWriter* textWriter) {
    mPageMain = new PageMain(heap);
    mPageMain->init();
    mPageOptions = new PageOptions(heap);
    mPageOptions->init();
    mPageHitSensor = new PageHitSensor(heap);
    mPageHitSensor->init();
    mPageInfo = new PageInfo(heap);
    mPageInfo->init();
    mPageTAS = new PageTAS(heap);
    mPageTAS->init();
    mPageAbout = new PageAbout(heap);
    mPageAbout->init();
    mCurPage = mPageMain;
    mTextWriter = textWriter;
}

void Menu::setCurPage(PageBase* page) {
    mCurPage->mCursorIndex = mCursorIndex;
    mCurPage = page;
    mCursorIndex = mCurPage->mCursorIndex;
}

void Menu::draw(al::Scene* scene) {
    mTextWriter->beginDraw();
    mTextWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 270.f));
    mTextWriter->setScaleFromFontHeight(20.f);
    mCurPage->draw(scene, mTextWriter);
    sead::Vector2f cursorPos = *mCurPage->mCursorPositions[mCursorIndex];
    mTextWriter->setCursorFromTopLeft(cursorPos);
    //    Logger::log("Cursor Pos: X: %.3f  Y: %.3f\n", cursorPos.x, cursorPos.y);
    mTextWriter->printf(">");
    mTextWriter->endDraw();
}

void Menu::handleInput() {
    int lines = mCurPage->mCursorPositions.size();
    if (isHandleInputs) {
        if (al::isPadTriggerUp(-1)) mCursorIndex--;
        if (al::isPadTriggerDown(-1)) mCursorIndex++;
        if (mCursorIndex < 0) mCursorIndex = lines - 1;
        if (mCursorIndex >= lines) mCursorIndex = 0;
        mCurPage->handleInput(mCursorIndex);
    }
    if (al::isPadTriggerPressLeftStick(-1)) isHandleInputs = !isHandleInputs;
}
