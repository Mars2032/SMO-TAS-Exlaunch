#include "debug-menu/Menu.h"
#include "smo-tas/TAS.h"

#define DEMANGLE(NAME, OUT)  char* OUT;                                                                              \
                            {                                                                                        \
                                char* demangleBuffer = static_cast<char*>(alloca(128));                               \
                                size_t demangledSize;                                                                \
                                int cxaStatus;                                                                       \
                                OUT = abi::__cxa_demangle(NAME, demangleBuffer, &demangledSize, &cxaStatus); \
                            }

void PageInfo::init() {
    addSelectableLine(21);
}

void PageInfo::handleInput(int cursorIndex) {
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

void PageInfo::draw(al::Scene* scene, sead::TextWriter* textWriter) {
    Menu* menu = Menu::instance();
    auto* tas = TAS::instance();
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 690.f));
    textWriter->printf("Return to Main     %s", menu->isHandleInputs ? "" : "[MENU DISABLED]");
    textWriter->printf("  %s\n", tas->isRunning() ? "[TAS RUNNING]" : "");
    textWriter->setCursorFromTopLeft(sead::Vector2f(20.f, 270.f));
    textWriter->printf("Info\n\n");
    PlayerActorBase* playerBase = rs::getPlayerActor(scene);
    if (!playerBase) {
        textWriter->printf("PlayerActorBase is null.\n");
        return;
    }
    DEMANGLE(typeid(*scene).name(), sceneName)
    textWriter->printf("Scene: %s\n\n", sceneName);
    DEMANGLE(typeid(*playerBase).name(), playerName)
    sead::Vector3f playerTrans = al::getTrans(playerBase);
    sead::Vector3f playerVel = al::getVelocity(playerBase);
    sead::Quatf playerQuat = al::getQuat(playerBase);
    sead::Vector3f playerRPY = sead::Vector3f::zero;
    al::calcQuatRotateDegree(&playerRPY, playerQuat);
    textWriter->printf("Player (%s):\n Pos:  (X: %.3f  Y: %.3f  Z: %.3f)\n", playerName, playerTrans.x, playerTrans.y, playerTrans.z);
    textWriter->printf(" Vel:  (X: %.3f  Y: %.3f  Z: %.3f)\n", playerVel.x, playerVel.y, playerVel.z);
    textWriter->printf(" Quat: (W: %.3f  X: %.3f  Y: %.3f  Z: %.3f)\n", playerQuat.w, playerQuat.x, playerQuat.y, playerQuat.z);
    textWriter->printf(" Rot:  (R: %.3f  P: %.3f  Y: %.3f)\n", playerRPY.x, playerRPY.y, playerRPY.z);
    al::NerveKeeper* playerNerveKeeper = playerBase->getNerveKeeper();
    if (playerNerveKeeper) {
        al::Nerve* playerNerve = playerNerveKeeper->getCurrentNerve();
        DEMANGLE(typeid(*playerNerve).name(), playerNerveName)
        textWriter->printf("Current Nerve: %s\n", playerNerveName+23+strlen(playerName)+3);
        if (playerNerveKeeper->mStateCtrl) {
            al::State* state = playerNerveKeeper->mStateCtrl->findStateInfo(playerNerve);
            if (state) {
                DEMANGLE(typeid(*state->mStateBase).name(), stateName)
                textWriter->printf("Current State: %s\n", stateName);
                al::Nerve* stateNerve = state->mStateBase->getNerveKeeper()->getCurrentNerve();
                DEMANGLE(typeid(*stateNerve).name(), stateNerveName)
                textWriter->printf("State Nerve: %s\n", stateNerveName+23+strlen(stateName)+3);
            }
        }
    } else {
        textWriter->printf("No NerveKeeper.\n\n\n");
    }
    textWriter->printf("\n");
    if (!al::isEqualString(typeid(*playerBase).name(), typeid(PlayerActorHakoniwa).name()))  {
        textWriter->printf("No HackCap in stage.\n");
        return;
    }
    PlayerActorHakoniwa* player = static_cast<PlayerActorHakoniwa*>(playerBase);
    HackCap* cap = player->mHackCap;
    DEMANGLE(typeid(*cap).name(), capName)
    sead::Vector3f capTrans = al::getTrans(cap);
    sead::Vector3f capVel = al::getVelocity(cap);
    sead::Vector3f capRPY = sead::Vector3f::zero;
    sead::Quatf capQuat = al::getQuat(cap);
    al::calcQuatRotateDegree(&capRPY, capQuat);
    textWriter->printf("Cappy (%s):\n Pos:  (X: %.3f  Y: %.3f  Z: %.3f)\n", capName, capTrans.x, capTrans.y, capTrans.z);
    textWriter->printf(" Vel:  (X: %.3f  Y: %.3f  Z: %.3f)\n", capVel.x, capVel.y, capVel.z);
    textWriter->printf(" Quat: (W: %.3f  X: %.3f  Y: %.3f  Z: %.3f)\n", capQuat.w, capQuat.x, capQuat.y, capQuat.z);
    textWriter->printf(" Rot:  (R: %.3f  P: %.3f  Y: %.3f)\n", capRPY.x, capRPY.y, capRPY.z);
    al::NerveKeeper* capNerveKeeper = cap->getNerveKeeper();
    if (capNerveKeeper) {
        al::Nerve* capNerve = capNerveKeeper->getCurrentNerve();
        DEMANGLE(typeid(*capNerve).name(), capNerveName)
        textWriter->printf("Current Nerve: %s\n", capNerveName+23+strlen(capName)+3);
    } else {
        textWriter->printf("No NerveKeeper.\n");
    }
}