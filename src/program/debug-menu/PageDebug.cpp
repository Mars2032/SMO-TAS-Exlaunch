#include "debug-menu/PageDebug.h"
#include "al/Pad/JoyPadAccelerometerAddon.h"
#include "al/Pad/PadGyroAddon.h"
#include "debug-menu/Menu.h"
#include "smo-tas/TAS.h"

void PageDebug::init() {
    addSelectableLine(21);
}

void PageDebug::handleInput(int cursorIndex) {
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

void PageDebug::draw(al::Scene* scene, sead::TextWriter* textWriter) {
    Menu* menu = Menu::instance();
    auto* tas = TAS::instance();
    textWriter->printf("Debug\n\n");
    sead::ControllerMgr* controllerMgr = sead::ControllerMgr::instance();
    sead::Controller* controller = controllerMgr->getController(al::getPlayerControllerPort(0));
    auto* accelLeft = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAccel, 0);
    auto* accelRight = (al::JoyPadAccelerometerAddon*)controller->getAddonByOrder(sead::ControllerDefine::cAccel, 1);
    auto* gyroLeft = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cGyro, 0);
    auto* gyroRight = (al::PadGyroAddon*)controller->getAddonByOrder(sead::ControllerDefine::cGyro, 1);
    textWriter->printf("Accel Left:\n  %.3f  %.3f  %.3f\n", accelLeft->mAcceleration.x, accelLeft->mAcceleration.y, accelLeft->mAcceleration.z);
    textWriter->printf("Accel Right:\n  %.3f  %.3f  %.3f\n", accelRight->mAcceleration.x, accelRight->mAcceleration.y, accelRight->mAcceleration.z);

    textWriter->printf("Gyro Left:\n  %.3f  %.3f  %.3f\n", gyroLeft->mDirection.a[0], gyroLeft->mDirection.a[1], gyroLeft->mDirection.a[2]);
    textWriter->printf("  %.3f  %.3f  %.3f\n", gyroLeft->mDirection.a[3], gyroLeft->mDirection.a[4], gyroLeft->mDirection.a[5]);
    textWriter->printf("  %.3f  %.3f  %.3f\n", gyroLeft->mDirection.a[6], gyroLeft->mDirection.a[7], gyroLeft->mDirection.a[8]);
    textWriter->printf("Gyro Right:\n  %.3f  %.3f  %.3f\n", gyroRight->mDirection.a[0], gyroRight->mDirection.a[1], gyroRight->mDirection.a[2]);
    textWriter->printf("  %.3f  %.3f  %.3f\n", gyroRight->mDirection.a[3], gyroLeft->mDirection.a[4], gyroRight->mDirection.a[5]);
    textWriter->printf("  %.3f  %.3f  %.3f\n", gyroRight->mDirection.a[6], gyroRight->mDirection.a[7], gyroRight->mDirection.a[8]);

    textWriter->printf("Left AngularV:\n  %.3f  %.3f  %.3f\n", gyroLeft->mAngularVel.x, gyroLeft->mAngularVel.y, gyroLeft->mAngularVel.z);
    textWriter->printf("Right AngularV:\n  %.3f  %.3f  %.3f\n", gyroRight->mAngularVel.x, gyroRight->mAngularVel.y, gyroRight->mAngularVel.z);

    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("Return to Main     %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    if (tas->isRunning()) {
            textWriter->printf("[TAS RUNNING %d/%d]", tas->getFrameIndex(), tas->getFrameCount());
    }
}