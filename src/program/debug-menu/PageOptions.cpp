#include "debug-menu/Menu.h"
#include "smo-tas/TAS.h"

void PageOptions::init() {
    addSelectableLine(4);
    addSelectableLine(5);
    addSelectableLine(6);
    addSelectableLine(21);
}

void PageOptions::handleInput(int cursorIndex) {
    Menu* menu = Menu::instance();
    if (!menu->isTriggerRight()) return;
    switch(cursorIndex) {
        case 0:
            menu->mCurPage = menu->mPageHitSensor;
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            menu->mCurPage = menu->mPageMain;
            break;
        default:
            break;
    }
}

void PageOptions::draw(al::Scene* scene, sead::TextWriter* textWriter) {
    sead::Color4f c;
    Menu* menu = Menu::instance();
    auto* tas = TAS::instance();
    c = {1.0, 1.0, 1.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("Options\n\n");
    textWriter->printf("Please note that changing any options WILL save the game.\n\n");
    textWriter->printf("  HitSensor Options\n  Collider Options\n  AreaObj Options\n");
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("Return to Main     %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    textWriter->printf("  %s\n", tas->isRunning() ? "[TAS RUNNING]" : "");
}