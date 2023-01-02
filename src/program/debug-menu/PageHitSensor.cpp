#include "al/util/MathUtil.h"
#include "debug-menu/Menu.h"
#include "game/GameData/SaveDataAccessFunction.h"
#include "smo-tas/TAS.h"

void PageHitSensor::init() {
    addSelectableLine(2);
    addSelectableLine(3);
    addSelectableLine(4);
    addSelectableLine(5);
    addSelectableLine(6);
    addSelectableLine(7);
    addSelectableLine(8);
    addSelectableLine(9);
    addSelectableLine(10);
    addSelectableLine(11);
    addSelectableLine(12);
    addSelectableLine(13);
    addSelectableLine(20);
    addSelectableLine(21);
}

void PageHitSensor::handleInput(int cursorIndex) {
    Menu* menu = Menu::instance();
    if (!menu->isTriggerRight() && !menu->isTriggerLeft()) return;
    auto* tas = TAS::instance();
    GameDataHolderAccessor accessor(tas->getScene());
    switch (cursorIndex) {
    case 0:
        menu->isShowSensors = !menu->isShowSensors;
        break;
    case 1:
        menu->isShowEyes = !menu->isShowEyes;
        break;
    case 2:
        menu->isShowAttack = !menu->isShowAttack;
        break;
    case 3:
        menu->isShowNpc = !menu->isShowNpc;
        break;
    case 4:
        menu->isShowBindable = !menu->isShowBindable;
        break;
    case 5:
        menu->isShowEnemyBody = !menu->isShowEnemyBody;
        break;
    case 6:
        menu->isShowMapObj = !menu->isShowMapObj;
        break;
    case 7:
        menu->isShowPlayerAll = !menu->isShowPlayerAll;
        break;
    case 8:
        menu->isShowHoldObj = !menu->isShowHoldObj;
        break;
    case 9:
        menu->isShowOther = !menu->isShowOther;
        break;
    case 10:
        if (menu->isTriggerLeft()) menu->mSensorAlpha -= 0.05;
        if (menu->isTriggerRight()) menu->mSensorAlpha += 0.05;
        menu->mSensorAlpha = al::clamp(menu->mSensorAlpha, 0.00f, 1.00f);
        break;
    case 11:
        menu->isShowTrans = !menu->isShowTrans;
        break;
    case 12:
        if (menu->isTriggerRight()) {
            menu->setCurPage(menu->mPageOptions);
            SaveDataAccessFunction::startSaveDataWriteSync(accessor.mData);
        }
        break;
    case 13:
        if (menu->isTriggerRight()) {
            menu->setCurPage(menu->mPageMain);
            SaveDataAccessFunction::startSaveDataWriteSync(accessor.mData);
        }
        break;
    default:
        break;
    }
}

void PageHitSensor::draw(al::Scene* scene, sead::TextWriter* textWriter) {
    sead::Color4f c;
    Menu* menu = Menu::instance();
    auto* tas = TAS::instance();
    textWriter->printf("HitSensor Options\n\n");
    textWriter->printf("Show HitSensors:                 %s\n", menu->isShowSensors ? "Enabled" : "Disabled");
    c = {0.0, 0.0, 1.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show Eye Sensors:              %s\n", menu->isShowEyes ? "Enabled" : "Disabled");
    c = {0.0, 0.707, 0.707, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show Attack Sensors:           %s\n", menu->isShowAttack ? "Enabled" : "Disabled");
    c = {0.0, 1.0, 0.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show Npc Sensors:              %s\n", menu->isShowNpc ? "Enabled" : "Disabled");
    c = {0.707, 0.707, 0.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show Bindable Sensors:         %s\n", menu->isShowBindable ? "Enabled" : "Disabled");
    c = {0.988, 0.549, 0.11, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show EnemyBody Sensors:        %s\n", menu->isShowEnemyBody ? "Enabled" : "Disabled");
    c = {0.0, 0.0, 1.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show MapObj Sensors:           %s\n", menu->isShowMapObj ? "Enabled" : "Disabled");
    c = {1.0, 0.0, 0.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show Player Sensors:           %s\n", menu->isShowPlayerAll ? "Enabled" : "Disabled");
    c = {0.404, 0.051, 0.839, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show HoldObj Sensors:          %s\n", menu->isShowHoldObj ? "Enabled" : "Disabled");
    c = {1.0, 1.0, 1.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("  Show Other Sensors:            %s\n", menu->isShowOther ? "Enabled" : "Disabled");
    textWriter->printf("  Alpha:  %s  %.2f  %s\n",
                       (menu->mCursorIndex == 10) ? (menu->isTriggerLeft() ? "< " : " <") : ("  "),
                       menu->mSensorAlpha,
                       (menu->mCursorIndex == 10) ? (menu->isTriggerRight() ? "  >" : "> ") : ("  "));
    c = {0.7922, 0.0, 1.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("Show Actor Positions:            %s\n", menu->isShowTrans ? "Enabled" : "Disabled");
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 670.f));
    c = {1.0, 1.0, 1.0, 1.0};
    textWriter->setColor(c);
    textWriter->printf("Return to Options\n");
    textWriter->printf("Return to Main     %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    if (tas->isRunning()) {
        textWriter->printf("[TAS RUNNING %d/%d]", tas->getFrameIndex(), tas->getFrameCount());
    }
}