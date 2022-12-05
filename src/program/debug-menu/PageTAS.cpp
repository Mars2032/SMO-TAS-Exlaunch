#include "al/util/NerveUtil.h"
#include "debug-menu/Menu.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "fs/fs_directories.hpp"
#include "smo-tas/TAS.h"

void PageTAS::init() {
    updateDir();
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
}

void PageTAS::updateDir() {
    nn::fs::DirectoryHandle handle;
    nn::Result r = nn::fs::OpenDirectory(&handle, "sd:/scripts", nn::fs::OpenDirectoryMode_File);
    if (R_FAILED(r)) return;
    s64 entryCount;
    r = nn::fs::GetDirectoryEntryCount(&entryCount, handle);
    if (R_FAILED(r)) {
        nn::fs::CloseDirectory(handle);
        return;
    }
    nn::fs::DirectoryEntry* entryBuffer = new nn::fs::DirectoryEntry[entryCount];
    r = nn::fs::ReadDirectory(&entryCount, entryBuffer, handle, entryCount);
    nn::fs::CloseDirectory(handle);
    if (R_FAILED(r)) {
        delete[] entryBuffer;
        return;
    }
    delete[] mEntries;
    mEntries = entryBuffer;
    mEntryCount = entryCount;
}

void PageTAS::handleInput(int cursorIndex) {
    if (!al::isPadTriggerRight(-1)) return;
    Menu* menu = Menu::instance();
    TAS* tas = TAS::instance();
    if (cursorIndex == 0) {
        if (tas->getScriptName())
            tas->startScript();
    }
    if (cursorIndex == 1) {
        tas->endScript();
    }
    if (cursorIndex == 2) {
        updateDir();
    }
    if (cursorIndex == 16) {
        menu->setCurPage(menu->mPageMain);
    }
    for (int i = 0; i < mEntryCount; i++) {
        if (cursorIndex == i+3) {
            tas->tryLoadScript(mEntries[i]);
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
    if (!mEntries)  {
        textWriter->printf("  No scripts.\n");
    } else {
        for (int i = 0; i < mEntryCount; i++) {
            textWriter->printf("  %s\n", mEntries[i].m_Name);
        }
    }
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("Return to Main     %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    textWriter->printf("  %s\n", tas->isRunning() ? "[TAS RUNNING]" : "");
}
