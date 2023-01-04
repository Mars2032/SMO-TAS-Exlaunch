#include "debug-menu/Menu.h"
#include "smo-tas/TAS.h"

void PageAbout::init() {
    addSelectableLine(21);
}

void PageAbout::handleInput(int cursorIndex) {
    Menu* menu = Menu::instance();
    if (!menu->isTriggerRight()) return;
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
    textWriter->printf("Version: 1.0.0\n\n");
    textWriter->printf("How to use:\n  D-Pad Up and Down control the cursor.\n  D-Pad Right to select.\n  L-Stick Press disables the menu.\n  Non-dpad inputs will not work when a TAS is running.\n");
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("Return to Main     %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    if (tas->isRunning()) {
            textWriter->printf("[TAS RUNNING %d/%d]", tas->getFrameIndex(), tas->getFrameCount());
    }
}