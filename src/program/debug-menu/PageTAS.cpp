#include "al/util/NerveUtil.h"
#include "debug-menu/Menu.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "fs/fs_directories.hpp"
#include "smo-tas/TAS.h"

void PageTAS::init() {
    //TAS* tas = TAS::instance();
    //tas->updateDir();
    addSelectableLine(2);
    addSelectableLine(3);
    addSelectableLine(5);
    addSelectableLine(7);
    addSelectableLine(8);
    addSelectableLine(9);
    addSelectableLine(10);
    addSelectableLine(11);
    addSelectableLine(12);
    addSelectableLine(13);
    addSelectableLine(14);
    addSelectableLine(15);
    addSelectableLine(16);
    addSelectableLine(17);
    addSelectableLine(18);
    addSelectableLine(19);
    addSelectableLine(21);
    //currentScriptEntry.m_Name = "";
}

void PageTAS::handleInput(int cursorIndex) {
    Menu* menu = Menu::instance();
    if (!menu->isTriggerRight()) return;
    TAS* tas = TAS::instance();
    if (cursorIndex == 0) {
        tas->tryStartScript();
    }
    if (cursorIndex == 1) {
        tas->endScript();
    }
    if (cursorIndex == 2) {
        tas->updateDir();
    }
    if (cursorIndex == 16) {
        menu->setCurPage(menu->mPageMain);
    }
    for (int i = 0; i < tas->mEntryCount; i++) {
        if (cursorIndex == i+3) {
            tas->currentScriptEntry = tas->mEntries[i];
            tas->tryLoadScript();
        }
    }
}


void PageTAS::draw(al::Scene* scene, sead::TextWriter* textWriter) {
    Menu* menu = Menu::instance();
    TAS* tas = TAS::instance();
    textWriter->printf("TAS\n\n");
    textWriter->printf("Start Script %s\n", tas->getScriptName());
    textWriter->printf("End Script\n\n");
    textWriter->printf("Refresh Directory\n");
    textWriter->printf("Scripts (located in sd:/scripts/):\n");
    if (!tas->mEntries)  {
        textWriter->printf("  No scripts.\n");
    } else {
        for (int i = 0; i < tas->mEntryCount; i++) {
            textWriter->printf("  %s\n", tas->mEntries[i].m_Name);
        }
    }
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("Return to Main     %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    if (tas->isRunning()) {
        textWriter->printf("[TAS RUNNING %d/%d]", tas->getFrameIndex(), tas->getFrameCount());
    }
}
