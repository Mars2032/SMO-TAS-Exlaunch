#include "debug-menu/Menu.h"
#include "smo-tas/TAS.h"

void PageMain::init() {
    addSelectableLine(3);
    addSelectableLine(4);
    addSelectableLine(5);
    addSelectableLine(6);
    addSelectableLine(7);
}

void PageMain::handleInput(int cursorIndex) {
    Menu* menu = Menu::instance();
    TAS* tas = TAS::instance();
    if (!menu->isTriggerRight()) return;
    switch (cursorIndex) {
    case 0:
        menu->setCurPage(menu->mPageOptions);
        break;
    case 1:
        menu->setCurPage(menu->mPageInfo);
        break;
    case 2:
        menu->setCurPage(menu->mPageTAS);
        tas->updateDir();
        break;
    case 3:
        menu->setCurPage(menu->mPageAbout);
        break;
    case 4:
        menu->setCurPage(menu->mPageDebug);
        break;
    default:
        break;
    }
}

void PageMain::draw(al::Scene* scene, sead::TextWriter* textWriter) {
    sead::Color4f c;
    Menu* menu = Menu::instance();
    auto* tas = TAS::instance();
    c = {1.0, 1.0, 1.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("Welcome to the SMO-TAS Mod!\n\n");
    textWriter->printf("Select a Page:\n");
    textWriter->printf("Options\nInfo\nTAS\nAbout\nDebug\n");
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("                   %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    if (tas->isRunning()) {
        textWriter->printf("[TAS RUNNING %d/%d]", tas->getFrameIndex(), tas->getFrameCount());
    }
}