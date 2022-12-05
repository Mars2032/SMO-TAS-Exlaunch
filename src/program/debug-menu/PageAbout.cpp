#include "debug-menu/Menu.h"
#include "smo-tas/TAS.h"

void PageAbout::init() {
    addSelectableLine(21);
}

void PageAbout::handleInput(int cursorIndex) {
    if (!al::isPadTriggerRight(-1)) return;
    Menu* menu = Menu::instance();
    switch (cursorIndex) {
        case 0:
            menu->setCurPage(menu->mPageMain);
            break;
        default:
            break;
    }
}

void PageAbout::draw(al::Scene* scene, sead::TextWriter* textWriter) {
    Menu* menu = Menu::instance();
    auto* tas = TAS::instance();
    textWriter->printf("Created by Mars2030#8008\nContributors:\n  tetraxile#0255\n  Sanae#4092\n  CraftyBoss#2586\n\n");
    textWriter->printf("Version: 1.0.0\n");
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("Return to Main     %s\n", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    textWriter->printf("  %s\n", tas->isRunning() ? "[TAS RUNNING]" : "");
}