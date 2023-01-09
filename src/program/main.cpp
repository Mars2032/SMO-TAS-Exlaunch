#include "agl/utl.h"
#include "al/LiveActor/LiveActorFlag.h"
#include "al/Pad/JoyPadAccelerometerAddon.h"
#include "al/Pad/PadGyroAddon.h"
#include "al/actor/LiveActorKit.h"
#include "al/byaml/writer/ByamlWriter.h"
#include "al/fs/FileLoader.h"
#include "al/util.hpp"
#include "debug-menu/ImGuiMenu.h"
#include "filedevice/nin/seadNinSDFileDeviceNin.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "fs.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "game/StageScene/StageScene.h"
#include "game/System/Application.h"
#include "game/System/GameSystem.h"
#include "gfx/seadPrimitiveRenderer.h"
#include "ghost/ghost-recorder.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "imgui_nvn.h"
#include "lib.hpp"
#include "logger/Logger.hpp"
#include "patches.hpp"
#include "rs/util.hpp"
#include "sead/basis/seadNew.h"
#include "smo-tas/TAS.h"
#include <cxxabi.h>
#include <devenv/seadDebugFontMgrNvn.h>
#include <gfx/seadTextWriter.h>
#include <gfx/seadViewport.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>

static const char *DBG_FONT_PATH   = "DebugData/Font/nvn_font_jis1.ntx";
static const char *DBG_SHADER_PATH = "DebugData/Font/nvn_font_shader_jis1.bin";
static const char *DBG_TBL_PATH    = "DebugData/Font/nvn_font_jis1_tbl.bin";
static sead::TextWriter* gTextWriter;

void drawHitSensors(al::LiveActor* actor) {
    auto* settings = Settings::instance();
    sead::PrimitiveRenderer* renderer = sead::PrimitiveRenderer::instance();
    if (actor->mPoseKeeper && settings->isShowTrans)
        renderer->drawSphere8x16(al::getTrans(actor), 10, sead::Color4f(0.7922*0.7922,0.0,1.0,1.0));
    al::HitSensorKeeper* sensorKeeper = actor->mHitSensorKeeper;
    if (sensorKeeper && settings->isShowSensors) {
        for (int i = 0; i < sensorKeeper->mSensorNum; i++) {
            al::HitSensor* curSensor = sensorKeeper->mSensors[i];
            if (curSensor) {
                if (al::isSensorValid(curSensor)) {
                    sead::Vector3f sensorTrans = al::getSensorPos(curSensor);
                    float sensorRadius = al::getSensorRadius(curSensor);
                    const char* sensorName = curSensor->mName;
                    sead::Color4f c;
                    c.r = 0.0;
                    c.g = 0.0;
                    c.b = 0.0;
                    c.a = settings->mSensorAlpha;
                    if (al::isSensorPlayerAttack(curSensor) || al::isSensorEnemyAttack(curSensor)
                        || al::isEqualString(sensorName, "Attack")) {
                        if (!settings->isShowAttack)
                            c.a = 0.0f;
                        c.g = 0.5;
                        c.b = 0.5; // cyan/light blue
                    } else if (al::isSensorEye(curSensor) || al::isSensorPlayerEye(curSensor)) {
                        c.b = 1.0; // blue circle
                        c.a = 0.9;
                        if (!settings->isShowEyes)
                            c.a = 0.0f;
                        renderer->drawCircle32(sensorTrans, sensorRadius, c);
                        return;
                    } else if (al::isEqualString("Trample", sensorName)) {
                        c.a = 0.0;
                    } else if (al::isSensorNpc(curSensor)) {
                        if (!settings->isShowNpc) c.a = 0.0f;
                        c.g = 1.0; // green
                    } else if (al::isSensorBindableAll(curSensor)) {
                        if (!settings->isShowBindable)
                            c.a = 0.0f;
                        c.r = 0.5;
                        c.g = 0.5; // yellow
                    } else if (al::isSensorEnemyBody(curSensor)) {
                        if (!settings->isShowEnemyBody)
                            c.a = 0.0f;
                        c.r = 0.988*0.988;
                        c.g = 0.549*0.549;
                        c.b = 0.11*0.11; // orange
                    } else if (al::isSensorMapObj(curSensor)) {
                        if (!settings->isShowMapObj) c.a = 0.0f;
                        c.b = 1.0; // blue
                    } else if (al::isSensorPlayerAll(curSensor)) {
                        if (!settings->isShowPlayerAll)
                            c.a = 0.0;
                        c.r = 1.0; // red
                    } else if (al::isSensorHoldObj(curSensor)) {
                        if (!settings->isShowHoldObj) c.a = 0.0;
                        c.r = 0.404*0.404;
                        c.g = 0.051*0.051;
                        c.b = 0.839*0.839; // purple
                    } else {
                        c.r = 1.0;
                        c.g = 1.0;
                        c.b = 1.0;
                        if (!settings->isShowOther) c.a = 0.0;
                    }
                    renderer->drawSphere8x16(sensorTrans, sensorRadius, c);
                }
            }
        }
    }

}

HOOK_DEFINE_TRAMPOLINE(DataReadHook) {
    static void Callback(void* thisPtr, const al::ByamlIter& iter) {
        Orig(thisPtr, iter);
        auto* settings = Settings::instance();
        al::tryGetByamlBool(&settings->isShowSensors, iter, "ShowSensors");
        al::tryGetByamlBool(&settings->isShowAttack, iter, "ShowAttack");
        al::tryGetByamlBool(&settings->isShowBindable, iter, "ShowBindable");
        al::tryGetByamlBool(&settings->isShowEnemyBody, iter, "ShowEnemyBody");
        al::tryGetByamlBool(&settings->isShowEyes, iter, "ShowEyes");
        al::tryGetByamlBool(&settings->isShowHoldObj, iter, "ShowHoldObj");
        al::tryGetByamlBool(&settings->isShowMapObj, iter, "ShowMapObj");
        al::tryGetByamlBool(&settings->isShowNpc, iter, "ShowNpc");
        al::tryGetByamlBool(&settings->isShowOther, iter, "ShowOther");
        al::tryGetByamlBool(&settings->isShowPlayerAll, iter, "ShowPlayerAll");
        al::tryGetByamlBool(&settings->isShowTrans, iter, "ShowTrans");
        al::tryGetByamlBool(&settings->isSkipCutscenes, iter, "SkipCutscenes");
        al::tryGetByamlF32(&settings->mSensorAlpha, iter, "SensorAlpha");
        al::tryGetByamlS32(&settings->mMofumofuPatternIndex, iter, "WigglerIndex");
        al::tryGetByamlBool(&settings->isEnableAutosave, iter, "AutoSave");
        al::tryGetByamlBool(&settings->isEnableMoonRefresh, iter, "MoonRefresh");
        al::tryGetByamlBool(&settings->isEnableGreyMoonRefresh, iter, "GreyMoonRefresh");
        al::tryGetByamlS64(&settings->mRandomSeed, iter, "RandomSeed");
    }
};

HOOK_DEFINE_TRAMPOLINE(DataWriteHook) {
    static void Callback(void* thisPtr, al::ByamlWriter* writer) {
        Orig(thisPtr, writer);
        auto* settings = Settings::instance();
        writer->addBool("ShowSensors", settings->isShowSensors);
        writer->addBool("ShowAttack", settings->isShowAttack);
        writer->addBool("ShowBindable", settings->isShowBindable);
        writer->addBool("ShowEnemyBody", settings->isShowEnemyBody);
        writer->addBool("ShowEyes", settings->isShowEyes);
        writer->addBool("ShowHoldObj", settings->isShowHoldObj);
        writer->addBool("ShowMapObj", settings->isShowMapObj);
        writer->addBool("ShowNpc", settings->isShowNpc);
        writer->addBool("ShowOther", settings->isShowOther);
        writer->addBool("ShowPlayerAll", settings->isShowPlayerAll);
        writer->addBool("ShowTrans", settings->isShowTrans);
        writer->addFloat("SensorAlpha", settings->mSensorAlpha);
        writer->addInt("WigglerIndex", settings->mMofumofuPatternIndex);
        writer->addBool("SkipCutscenes", settings->isSkipCutscenes);
        writer->addBool("AutoSave", settings->isEnableAutosave);
        writer->addBool("GreyMoonRefresh", settings->isEnableGreyMoonRefresh);
        writer->addBool("MoonRefresh", settings->isEnableMoonRefresh);
        writer->addInt64("RandomSeed", settings->mRandomSeed);
    }
};

HOOK_DEFINE_TRAMPOLINE(SceneMovementHook) {
    static void Callback(al::Scene* scene) {
        auto* tas = TAS::instance();
        tas->setScene(scene);
        tas->updateNerve();
//        auto* ghostRecord = GhostRecorder::instance();
//        ghostRecord->updateNerve();
        Orig(scene);
    }
};

HOOK_DEFINE_TRAMPOLINE(ControlHook) {
    static void Callback(StageScene* scene) {
        Orig(scene);
    }
};

HOOK_DEFINE_TRAMPOLINE(CreateFileDeviceMgr) {
    static void Callback(sead::FileDeviceMgr *thisPtr) {
        Orig(thisPtr);

        thisPtr->mMountedSd = nn::fs::MountSdCardForDebug("sd").isSuccess();
        sead::NinSDFileDevice *sdFileDevice = new sead::NinSDFileDevice();
        thisPtr->mount(sdFileDevice);
    }
};

HOOK_DEFINE_TRAMPOLINE(GameSystemInit) {
    static void Callback(GameSystem *thisPtr) {
        // setup TextWriter
        sead::Heap* curHeap = sead::HeapMgr::instance()->getCurrentHeap();
        sead::DebugFontMgrJis1Nvn::createInstance(curHeap);
        if (al::isExistFile(DBG_SHADER_PATH) && al::isExistFile(DBG_FONT_PATH) && al::isExistFile(DBG_TBL_PATH)) {
            sead::DebugFontMgrJis1Nvn::sInstance->initialize(curHeap, DBG_SHADER_PATH, DBG_FONT_PATH, DBG_TBL_PATH, 0x100000);
        }
        sead::TextWriter::setDefaultFont(sead::DebugFontMgrJis1Nvn::sInstance);
        al::GameDrawInfo* drawInfo = Application::instance()->mDrawInfo;
        agl::DrawContext *context = drawInfo->mDrawContext;
        agl::RenderBuffer* renderBuffer = drawInfo->getRenderBuffer();
        sead::Viewport* viewport = new sead::Viewport(*renderBuffer);
        gTextWriter = new sead::TextWriter(context, viewport);
        sead::TextWriter::setupGraphics(context);
        gTextWriter->mColor = sead::Color4f(1.f, 1.f, 1.f, 0.8f);
        // initalize TAS
        TAS::createInstance(curHeap);
        // initialize Ghost Recorder
        //GhostRecorder::createInstance(curHeap);
        // call original function
        Orig(thisPtr);

    }
};

class InitializeArg;

HOOK_DEFINE_TRAMPOLINE(GlobalRandomInit) {
    static void Callback(InitializeArg* thisPtr) {
        // initialize Settings used by ImGui
        Logger::log("Global Random Init\n");
        sead::Heap* heap = sead::HeapMgr::sRootHeaps[0];
        Settings::createInstance(heap);
        Logger::log("Settings Instance created!\n");
        Orig(thisPtr);
    }
};

HOOK_DEFINE_TRAMPOLINE(DrawDebugMenu) {
    static void Callback(HakoniwaSequence *thisPtr) {
        Orig(thisPtr);

        al::Scene* curScene = thisPtr->mCurrentScene;
        agl::DrawContext* drawContext = thisPtr->getDrawInfo()->mDrawContext;

        // Only runs if scene exists and HakoniwaSequence nerve doesn't contain Destroy
        if (curScene && !strstr(typeid(*al::getCurrentNerve(thisPtr)).name(), "Destroy")) {
            if (curScene->mLiveActorKit) {
                const sead::LookAtCamera* cam = al::getLookAtCamera(curScene, 0);
                const sead::Projection* proj = al::getProjectionSead(curScene, 0);
                sead::PrimitiveRenderer* renderer = sead::PrimitiveRenderer::instance();
                renderer->setDrawContext(drawContext);
                renderer->setCamera(*cam);
                renderer->setProjection(*proj);
                renderer->setModelMatrix(sead::Matrix34f::ident);
                al::LiveActorGroup* actorGroup = curScene->mLiveActorKit->mLiveActorGroup2;
                if (actorGroup) {
                    for (int i = 0; i < actorGroup->mActorCount; i++) {
                        al::LiveActor* curActor = actorGroup->mActors[i];
                        if (curActor) {
                            if (!curActor->mLiveActorFlag->mDead &&
                                (!curActor->mLiveActorFlag->mClipped || curActor->mLiveActorFlag->mDrawClipped)) {
                                renderer->begin();
                                drawHitSensors(curActor);
                                renderer->end();
                            }
                        }
                    }
                }
            }
        }
        // fps in top left
        gTextWriter->beginDraw();

        gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, 10.f));
        gTextWriter->printf("FPS: %.2f\n", Application::instance()->mFramework->calcFps());

        gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, (al::getLayoutDisplayHeight() / 3) + 30.f));
        gTextWriter->setScaleFromFontHeight(20.f);

        gTextWriter->endDraw();
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();
    R_ABORT_UNLESS(Logger::instance().init(LOGGER_IP, 3080).value);
    runCodePatches();
    GameSystemInit::InstallAtOffset(0x535850);
    GlobalRandomInit::InstallAtSymbol("_ZN4sead12GlobalRandom14createInstanceEPNS_4HeapE");

    // Debug Text Writer Drawing
    DrawDebugMenu::InstallAtOffset(0x50F1D8);

    // TAS
    SceneMovementHook::InstallAtSymbol("_ZN2al5Scene8movementEv");

    // File Device Mgr
    CreateFileDeviceMgr::InstallAtSymbol("_ZN4sead13FileDeviceMgrC1Ev");

    // Save file read/write
    DataReadHook::InstallAtSymbol("_ZN14GameConfigData4readERKN2al9ByamlIterE");
    DataWriteHook::InstallAtSymbol("_ZN14GameConfigData5writeEPN2al11ByamlWriterE");

    ControlHook::InstallAtSymbol("_ZN10StageScene7controlEv");

    // ImGUI hooks
    nvnImGui::InstallHooks();
    nvnImGui::addDrawFunc(drawPageMain);
    optionsHooks();

}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}