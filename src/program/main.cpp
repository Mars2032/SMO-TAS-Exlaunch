#include "agl/utl.h"
#include "al/LiveActor/LiveActorFlag.h"
#include "al/Pad/JoyPadAccelerometerAddon.h"
#include "al/Pad/PadGyroAddon.h"
#include "al/actor/LiveActorKit.h"
#include "al/byaml/writer/ByamlWriter.h"
#include "al/fs/FileLoader.h"
#include "al/util.hpp"
#include "debug-menu/Menu.h"
#include "filedevice/nin/seadNinSDFileDeviceNin.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "fs.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "game/StageScene/StageScene.h"
#include "game/System/Application.h"
#include "game/System/GameSystem.h"
#include "gfx/seadPrimitiveRenderer.h"
#include "lib.hpp"
#include "logger/Logger.hpp"
#include "patches.hpp"
#include "rs/util.hpp"
#include "smo-tas/TAS.h"
#include <devenv/seadDebugFontMgrNvn.h>
#include <gfx/seadTextWriter.h>
#include <gfx/seadViewport.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>

static const char *DBG_FONT_PATH   = "DebugData/Font/nvn_font_jis1.ntx";
static const char *DBG_SHADER_PATH = "DebugData/Font/nvn_font_shader_jis1.bin";
static const char *DBG_TBL_PATH    = "DebugData/Font/nvn_font_jis1_tbl.bin";

void drawBackground(agl::DrawContext* context) {
    sead::Vector3<float> p1;  // top left
    p1.x = -1.0;
    p1.y = 0.3;
    p1.z = 0.0;
    sead::Vector3<float> p2;  // top right
    p2.x = 0.0;
    p2.y = 0.3;
    p2.z = 0.0;
    sead::Vector3<float> p3;  // bottom left
    p3.x = -1.0;
    p3.y = -1.0;
    p3.z = 0.0;
    sead::Vector3<float> p4;  // bottom right
    p4.x = 0.0;
    p4.y = -1.0;
    p4.z = 0.0;

    sead::Color4f c;
    c.r = 0.1;
    c.g = 0.1;
    c.b = 0.1;
    c.a = 0.75;

    agl::utl::DevTools::beginDrawImm(context, sead::Matrix34<float>::ident,
                                     sead::Matrix44<float>::ident);
    agl::utl::DevTools::drawTriangleImm(context, p3, p2, p1, c);
    agl::utl::DevTools::drawTriangleImm(context, p3, p4, p2, c);
}

void drawHitSensors(al::LiveActor* actor) {
    Menu* menu = Menu::instance();
    if (!menu->isShowSensors) return;
    sead::PrimitiveRenderer* renderer = sead::PrimitiveRenderer::instance();
    if (actor->mPoseKeeper && menu->isShowTrans)
        renderer->drawSphere8x16(al::getTrans(actor), 10, sead::Color4f(0.7922*0.7922,0.0,1.0,1.0));
    al::HitSensorKeeper* sensorKeeper = actor->mHitSensorKeeper;
    if (sensorKeeper) {
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
                    c.a = menu->mSensorAlpha;
                    if (al::isSensorPlayerAttack(curSensor) || al::isSensorEnemyAttack(curSensor)
                        || al::isEqualString(sensorName, "Attack")) {
                        if (!menu->isShowAttack)
                            c.a = 0.0f;
                        c.g = 0.5;
                        c.b = 0.5; // cyan/light blue
                    } else if (al::isSensorEye(curSensor) || al::isSensorPlayerEye(curSensor)) {
                        c.b = 1.0; // blue circle
                        c.a = 0.9;
                        if (!menu->isShowEyes)
                            c.a = 0.0f;
                        renderer->drawCircle32(sensorTrans, sensorRadius, c);
                        return;
                    } else if (al::isEqualString("Trample", sensorName)) {
                        c.a = 0.0;
                    } else if (al::isSensorNpc(curSensor)) {
                        if (!menu->isShowNpc) c.a = 0.0f;
                        c.g = 1.0; // green
                    } else if (al::isSensorBindableAll(curSensor)) {
                        if (!menu->isShowBindable)
                            c.a = 0.0f;
                        c.r = 0.5;
                        c.g = 0.5; // yellow
                    } else if (al::isSensorEnemyBody(curSensor)) {
                        if (!menu->isShowEnemyBody)
                            c.a = 0.0f;
                        c.r = 0.988*0.988;
                        c.g = 0.549*0.549;
                        c.b = 0.11*0.11; // orange
                    } else if (al::isSensorMapObj(curSensor)) {
                        if (!menu->isShowMapObj) c.a = 0.0f;
                        c.b = 1.0; // blue
                    } else if (al::isSensorPlayerAll(curSensor)) {
                        if (!menu->isShowPlayerAll)
                            c.a = 0.0;
                        c.r = 1.0; // red
                    } else if (al::isSensorHoldObj(curSensor)) {
                        if (!menu->isShowHoldObj) c.a = 0.0;
                        c.r = 0.404*0.404;
                        c.g = 0.051*0.051;
                        c.b = 0.839*0.839; // purple
                    } else {
                        c.r = 1.0;
                        c.g = 1.0;
                        c.b = 1.0;
                        c.a = 0.05;
                        if (!menu->isShowOther) c.a = 0.0;
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
        auto* menu =  Menu::instance();
        al::tryGetByamlBool(&menu->isShowSensors, iter, "ShowSensors");
        al::tryGetByamlBool(&menu->isShowAttack, iter, "ShowAttack");
        al::tryGetByamlBool(&menu->isShowBindable, iter, "ShowBindable");
        al::tryGetByamlBool(&menu->isShowEnemyBody, iter, "ShowEnemyBody");
        al::tryGetByamlBool(&menu->isShowEyes, iter, "ShowEyes");
        al::tryGetByamlBool(&menu->isShowHoldObj, iter, "ShowHoldObj");
        al::tryGetByamlBool(&menu->isShowMapObj, iter, "ShowMapObj");
        al::tryGetByamlBool(&menu->isShowNpc, iter, "ShowNpc");
        al::tryGetByamlBool(&menu->isShowOther, iter, "ShowOther");
        al::tryGetByamlBool(&menu->isShowPlayerAll, iter, "ShowPlayerAll");
        al::tryGetByamlBool(&menu->isShowTrans, iter, "ShowTrans");
        al::tryGetByamlF32(&menu->mSensorAlpha, iter, "SensorAlpha");
    }
};

HOOK_DEFINE_TRAMPOLINE(DataWriteHook) {
    static void Callback(void* thisPtr, al::ByamlWriter* writer) {
        Orig(thisPtr, writer);
        auto* menu =  Menu::instance();
        writer->addBool("ShowSensors", menu->isShowSensors);
        writer->addBool("ShowAttack", menu->isShowAttack);
        writer->addBool("ShowBindable", menu->isShowBindable);
        writer->addBool("ShowEnemyBody", menu->isShowEnemyBody);
        writer->addBool("ShowEyes", menu->isShowEyes);
        writer->addBool("ShowHoldObj", menu->isShowHoldObj);
        writer->addBool("ShowMapObj", menu->isShowMapObj);
        writer->addBool("ShowNpc", menu->isShowNpc);
        writer->addBool("ShowOther", menu->isShowOther);
        writer->addBool("ShowPlayerAll", menu->isShowPlayerAll);
        writer->addBool("ShowTrans", menu->isShowTrans);
        writer->addFloat("SensorAlpha", menu->mSensorAlpha);
    }
};

HOOK_DEFINE_TRAMPOLINE(SceneMovementHook) {
    static void Callback(al::Scene* scene) {
        Menu* menu = Menu::instance();
        TAS* tas = TAS::instance();
        menu->handleInput();
        tas->setScene(scene);
        tas->updateNerve();
        Orig(scene);
    }
};

HOOK_DEFINE_TRAMPOLINE(SnapshotHook) {
    static bool Callback(al::IUseSceneObjHolder const* holder) {
        Menu* menu = Menu::instance();
        if (menu->isHandleInputs) return false;
        return Orig(holder);
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
        sead::TextWriter* textWriter = new sead::TextWriter(context, viewport);
        textWriter->setupGraphics(context);
        textWriter->mColor = sead::Color4f(1.f, 1.f, 1.f, 0.8f);
        // initialize menu
        Menu* menu = Menu::createInstance(curHeap);
        menu->init(curHeap, textWriter);
        // initalize TAS
        TAS::createInstance(curHeap);
        // call original function
        Orig(thisPtr);

    }
};

HOOK_DEFINE_TRAMPOLINE(DrawDebugMenu) {
    static void Callback(HakoniwaSequence *thisPtr) {
        Orig(thisPtr);

        int dispHeight = al::getLayoutDisplayHeight();
        agl::DrawContext* drawContext = thisPtr->getDrawInfo()->mDrawContext;
        drawBackground(drawContext);
        Menu* menu = Menu::instance();
        al::Scene* curScene = thisPtr->mCurrentScene;

        if (curScene) {
            sead::LookAtCamera* cam = al::getLookAtCamera(curScene, 0);
            sead::Projection* proj = al::getProjectionSead(curScene, 0);
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
                        if (!curActor->mLiveActorFlag->mDead && (!curActor->mLiveActorFlag->mClipped || curActor->mLiveActorFlag->mDrawClipped)) {
                            renderer->begin();
                            drawHitSensors(curActor);
                            renderer->end();
                        }
                    }
                }
            }
            menu->draw(curScene);
        }

        menu->mTextWriter->beginDraw();

        menu->mTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, 10.f));
        menu->mTextWriter->printf("FPS: %.2f\n", Application::instance()->mFramework->calcFps());

        menu->mTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, (dispHeight / 3) + 30.f));
        menu->mTextWriter->setScaleFromFontHeight(20.f);

        menu->mTextWriter->endDraw();
    }
};

extern "C" void exl_main(void* x0, void* x1) {
    /* Setup hooking enviroment. */
    envSetOwnProcessHandle(exl::util::proc_handle::Get());
    exl::hook::Initialize();
    R_ABORT_UNLESS(Logger::instance().init(LOGGER_IP, 3080).value);
    runCodePatches();
    GameSystemInit::InstallAtOffset(0x535850);

    // Debug Text Writer Drawing
    DrawDebugMenu::InstallAtOffset(0x50F1D8);

    // TAS
    SceneMovementHook::InstallAtSymbol("_ZN2al5Scene8movementEv");

    // File Device Mgr
    CreateFileDeviceMgr::InstallAtSymbol("_ZN4sead13FileDeviceMgrC1Ev");

    // Save file read/write
    DataReadHook::InstallAtSymbol("_ZN14GameConfigData4readERKN2al9ByamlIterE");
    DataWriteHook::InstallAtSymbol("_ZN14GameConfigData5writeEPN2al11ByamlWriterE");

    SnapshotHook::InstallAtSymbol("_ZN2rs21isTriggerSnapShotModeEPKN2al18IUseSceneObjHolderE");

    ControlHook::InstallAtSymbol("_ZN10StageScene7controlEv");

}

extern "C" NORETURN void exl_exception_entry() {
    /* TODO: exception handling */
    EXL_ABORT(0x420);
}