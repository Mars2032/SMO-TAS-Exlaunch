#include "ImGuiMenu.h"
#include "al/scene/Scene.h"
#include "al/sequence/Sequence.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/OtherUtil.h"
#include "game/GameData/SaveDataAccessFunction.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "game/Player/PlayerActorBase.h"
#include "game/System/GameSystem.h"
#include "helpers/PlayerHelper.h"
#include "imgui_backend/imgui_impl_nvn.hpp"
#include "logger/Logger.hpp"
#include "random/seadGlobalRandom.h"
#include "sead/basis/seadNew.h"
#include "smo-tas/TAS.h"
#include <cxxabi.h>

SEAD_SINGLETON_DISPOSER_IMPL(Settings)
Settings::Settings() = default;
Settings::~Settings() = default;

using namespace ImGui;

static bool isShowOptions = false;
static bool isShowInfo = false;
static bool isShowTAS = false;
static bool isShowDebug = false;
static bool isShowAbout = false;
static bool isShowHitSensor = false;
static bool isShowCollider = false;
static bool isShowAreaObj = false;
static bool isShowTASOptions = false;
static int scenario = -1;

void drawPageMain() {
    if (Begin("Main")) {
        Text("Welcome to the SMO-TAS Mod!\n");
        if (CollapsingHeader("Options")) {
            al::Scene* scene = GameSystemFunction::getGameSystem()->mSequence->mCurrentScene;
            if (scene) {
                GameDataHolderAccessor accessor(scene);
                if (Button("Save Options"))
                    SaveDataAccessFunction::startSaveDataWriteSync(accessor.mData);
            } else {
                Text("Cannot save, scene is null.");
            }
            if (Button("General Options"))
                isShowTASOptions = true;
            if (Button("HitSensor Options")) {
                isShowHitSensor = true;
            }
            if (Button("Collider Options")) {
                isShowCollider = true;
            }
            if (Button("AreaObj Options")) {
                isShowAreaObj = true;
            }
        }
        if (Button("Info"))
            isShowInfo = true;
        if (Button("TAS"))
            isShowTAS = true;
        if (Button("Debug"))
            isShowDebug = true;
        if (Button("About"))
            isShowAbout = true;

    }
    End();
    drawPageInfo();
    drawPageTAS();
    drawPageDebug();
    drawPageAbout();
    drawPageHitSensor();
    drawPageGeneralOptions();
}

void drawPageAbout() {
    if (!isShowAbout) return;
    if (Begin("About", &isShowAbout, ImGuiWindowFlags_NoResize)) {
        Text("Creators:");
        BulletText("Mars2030#8008");
        BulletText("tetraxile#0255");
        BulletText("Sanae#4092");
        BulletText("CraftyBoss#2586");
        BulletText("Xiivler#8167");
        Text("Mod Version: 1.1.0");
    }
    End();
}

void drawPageHitSensor() {
    if (!isShowHitSensor) return;
    if (Begin("Renderer Options", &isShowHitSensor, ImGuiWindowFlags_NoResize)) {
        auto* settings = Settings::instance();
        Checkbox("Show All HitSensors", &settings->isShowSensors);
        Checkbox("Show Eye Sensors", &settings->isShowEyes);
        Checkbox("Show Attack Sensors", &settings->isShowAttack);
        Checkbox("Show Npc Sensors", &settings->isShowNpc);
        Checkbox("Show Bindable Sensors", &settings->isShowBindable);
        Checkbox("Show EnemyBody Sensors", &settings->isShowEnemyBody);
        Checkbox("Show MapObj Sensors", &settings->isShowMapObj);
        Checkbox("Show Player Sensors", &settings->isShowPlayerAll);
        Checkbox("Show Other Sensors", &settings->isShowOther);
        SliderFloat("Sensor Alpha", &settings->mSensorAlpha, 0.0f, 1.0f);
        Checkbox("Show Actor Positions", &settings->isShowTrans);
    }
    End();
}

void drawPageGeneralOptions() {
    if (!isShowTASOptions) return;
    if (Begin("General Options", &isShowTASOptions)) {
        auto* settings = Settings::instance();
        Checkbox("Make Moons Re-Collectable", &settings->isEnableMoonRefresh);
        Checkbox("Make Moons Always Collectable", &settings->isEnableGreyMoonRefresh);
        Checkbox("Auto-Saving", &settings->isEnableAutosave);
        Checkbox("Make Cutscenes Always Skippable", &settings->isSkipCutscenes);

        const char* previewValue = Settings::mPatternEntries[settings->mMofumofuPatternIndex].type;
        if (BeginCombo("Mechawiggler Patterns", previewValue)) {
            for (int i = 0; i < IM_ARRAYSIZE(Settings::mPatternEntries); i++) {
                const bool isSelected = (settings->mMofumofuPatternIndex == i);
                if (Selectable(Settings::mPatternEntries[i].type, isSelected))
                    settings->mMofumofuPatternIndex = i;
                if (isSelected)
                    SetItemDefaultFocus();
            }
            EndCombo();
        }
        if (SliderInt("Random Seed", (int*)&settings->mRandomSeed, -1, INT32_MAX/2)) {
            sead::GlobalRandom::instance()->init(settings->mRandomSeed);
        }
        al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mSequence;
        if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
            auto gameSeq = (HakoniwaSequence *) curSequence;
            auto curScene = gameSeq->curScene;
            WorldResourceLoader* loader = gameSeq->mResourceLoader;

            bool isInGame =
                curScene && curScene->mIsAlive && al::isEqualString(curScene->mName.cstr(), "StageScene");
            if (isInGame)
                Text("Scene Name: %s", ((StageScene*)(curScene))->seedText);
            if (ImGui::CollapsingHeader("World List")) {
                SliderInt("Scenario", &scenario, -1, 15);
                for (auto &entry: gameSeq->mGameDataHolder.mData->mWorldList->mWorldList) {
                    if (ImGui::TreeNode(entry.mMainStageName)) {
                        if (isInGame) {
                            if (ImGui::Button("Warp to World")) {
                                if (scenario == 0) scenario = -1;
                                PlayerHelper::warpPlayer(entry.mMainStageName, gameSeq->mGameDataHolder, scenario);
                                loader->tryDestroyWorldResource();
                            }
                        }

                        ImGui::BulletText("Clear Main Scenario: %d", entry.mClearMainScenario);
                        ImGui::BulletText("Ending Scenario: %d", entry.mEndingScenario);
                        ImGui::BulletText("Moon Rock Scenario: %d", entry.mMoonRockScenario);
                        if (ImGui::TreeNode("Main Quest Infos")) {
                            for (int i = 0; i < entry.mQuestInfoCount; ++i) {
                                ImGui::BulletText("Quest %d Scenario: %d", i, entry.mMainQuestIndexes[i]);
                            }
                            ImGui::TreePop();
                        }
                        if (ImGui::CollapsingHeader("Database Entries")) {
                            for (auto &dbEntry: entry.mStageNames) {
                                if (ImGui::TreeNode(dbEntry.mStageName.cstr())) {
                                    ImGui::BulletText("Stage Category: %s", dbEntry.mStageCategory.cstr());
                                    ImGui::BulletText("Stage Use Scenario: %d", dbEntry.mUseScenario);

                                    if (isInGame) {
                                        ImGui::Bullet();
                                        if (ImGui::SmallButton("Warp to Stage")) {
                                            if (scenario == 0) scenario = -1;
                                            PlayerHelper::warpPlayer(dbEntry.mStageName.cstr(),
                                                                     gameSeq->mGameDataHolder, scenario);
                                            loader->tryDestroyWorldResource();
                                        }
                                    }
                                    ImGui::TreePop();
                                }
                            }
                        }
                        ImGui::TreePop();
                    }
                }
            }
            if (isInGame) {
                StageScene *stageScene = (StageScene *) gameSeq->curScene;
                PlayerActorBase *playerBase = rs::getPlayerActor(stageScene);

                if (ImGui::Button("Kill Mario")) {
                    PlayerHelper::killPlayer(playerBase);
                }
            }
        }
    }
    End();
}

void drawPageInfo() {
    if (!isShowInfo)
        return;
    if (Begin("Info", &isShowInfo)) {
        al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mSequence;
        al::Scene* scene = curSequence->mCurrentScene;
        if (CollapsingHeader("General Info")) {
            //Logger::log("Current Sequence: %s\n", typeid(*curSequence).name());
            int status;
            char* name;
            name = abi::__cxa_demangle(typeid(*curSequence).name(), nullptr, nullptr, &status);
            Text("Current Sequence: %s", name);
            free(name);
            if (scene) {
                name = abi::__cxa_demangle(typeid(*scene).name(), nullptr, nullptr, &status);
                Text("Current Scene: %s", name);
                free(name);
            } else {
                Text("Scene is null.");
            }
        }
        if (CollapsingHeader("Player Info")) {
            if (scene) {
                PlayerActorBase* playerBase = rs::getPlayerActor(scene);
                if (playerBase) {
                    sead::Vector3f playerTrans = al::getTrans(playerBase);
                    sead::Vector3f playerVel = al::getVelocity(playerBase);
                    float playerSpeed = al::calcSpeed(playerBase);
                    float playerSpeedH = al::calcSpeedH(playerBase);
                    sead::Quatf playerQuat = al::getQuat(playerBase);
                    sead::Vector3f playerRPY = sead::Vector3f::zero;
                    al::calcQuatRotateDegree(&playerRPY, playerQuat);
                    int status;
                    char* name;
                    size_t bitch = 512;
                    name = abi::__cxa_demangle(typeid(*playerBase).name(), (char*)alloca(bitch), &bitch, &status);
                    Text("Player Name: %s", name);
                    InputFloat3("Position", (float*)&playerTrans, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    InputFloat3("Velocity", (float*)&playerVel, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    InputFloat("Total Speed", &playerSpeed, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    InputFloat("Horizontal Speed", &playerSpeedH, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    InputFloat4("Quat Rotation", (float*)&playerQuat, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    InputFloat3("Euler Angles", (float*)&playerRPY, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    al::NerveKeeper* playerNerveKeeper = playerBase->getNerveKeeper();
                    if (playerNerveKeeper) {
                        al::Nerve* playerNerve = playerNerveKeeper->getCurrentNerve();
                        char* name2;
                        name2 = abi::__cxa_demangle(typeid(*playerNerve).name(), (char*)alloca(bitch), &bitch, &status);
                        Text("Current Nerve: %s", name2 + 23 + strlen(name) + 3);
                        if (playerNerveKeeper->mStateCtrl) {
                            al::State* state = playerNerveKeeper->mStateCtrl->findStateInfo(playerNerve);
                            if (state) {
                                name = abi::__cxa_demangle(typeid(*state->mStateBase).name(), (char*)alloca(bitch), &bitch, &status);
                                Text("Current State: %s", name);
                                al::Nerve* stateNerve = state->mStateBase->getNerveKeeper()->getCurrentNerve();
                                name2 = abi::__cxa_demangle(typeid(*stateNerve).name(), (char*)alloca(bitch), &bitch, &status);
                                Text("State Nerve: %s", name2 + 23 + strlen(name) + 3);
                            }
                        }
                    } else {
                        Text("NerveKeeper is null.");
                    }
                    if (CollapsingHeader("Cappy Info")) {
                        if (!al::isEqualString(typeid(*playerBase).name(), typeid(PlayerActorHakoniwa).name()))  {
                            Text("No HackCap in stage.\n");
                        } else {
                            auto* player = static_cast<PlayerActorHakoniwa*>(playerBase);
                            HackCap* cap = player->mHackCap;
                            name = abi::__cxa_demangle(typeid(*cap).name(), (char*)alloca(bitch), &bitch, &status);
                            sead::Vector3f capTrans = al::getTrans(cap);
                            sead::Vector3f capVel = al::getVelocity(cap);
                            float capSpeed = al::calcSpeed(cap);
                            float capSpeedH = al::calcSpeedH(cap);
                            sead::Vector3f capRPY = sead::Vector3f::zero;
                            sead::Quatf capQuat = al::getQuat(cap);
                            al::calcQuatRotateDegree(&capRPY, capQuat);
                            Text("Cappy Name: %s", name);
                            InputFloat3("Position", (float*)&capTrans, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            InputFloat3("Velocity", (float*)&capVel, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            InputFloat("Total Speed", &capSpeed, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            InputFloat("Horizontal Speed", &capSpeedH, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            InputFloat4("Quat Rotation", (float*)&capQuat, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            InputFloat3("Euler Angles", (float*)&capRPY, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            al::NerveKeeper* capNerveKeeper = cap->getNerveKeeper();
                            if (capNerveKeeper) {
                                al::Nerve* capNerve = capNerveKeeper->getCurrentNerve();
                                char* name2;
                                name2 = abi::__cxa_demangle(typeid(*capNerve).name(), (char*)alloca(bitch), &bitch, &status);
                                Text("Current Nerve: %s", name2+23+strlen(name)+3);
                                free(name);
                                free(name2);
                            } else {
                                Text("No NerveKeeper.");
                                free(name);
                            }
                        }
                    }


                } else {
                    Text("Player is null.");
                }
            } else {
                Text("Scene is null.");
            }
        }

    }
    End();
}

void drawPageTAS() {
    if (!isShowTAS) return;
    if (Begin("TAS", &isShowTAS)) {
        TAS* tas = TAS::instance();
        Text("Loaded Script: %s", tas->hasScript() ? tas->getScriptName() : "None.");
        if (Button("Start TAS"))
            tas->tryStartScript();
        if (Button("End TAS"))
            tas->endScript();
        if (Button("Refresh Directory"))
            tas->updateDir();
        //Logger::log("1 Script Name: %s\n", tas->getScriptName());

        if(CollapsingHeader("Scripts")) {
            s64 entryCount = tas->getEntryCount();
            nn::fs::DirectoryEntry* entries = tas->getScripts();
            for (int i = 0; i < entryCount; i++) {
                if (Button(entries[i].m_Name)) {
                    //Logger::log("2 Script Name: %s\n", tas->getScriptName());
                    tas->setScript(entries[i]);
                    //Logger::log("3 Script Name: %s\n", tas->getScriptName());
                    tas->tryLoadScript();
                }
            }
        }
        if (tas->isRunning()) {
            Text("TAS is running (%d/%d)", tas->getFrameIndex(), tas->getFrameCount());
        } else {
            Text("TAS is not running.");
        }
    }
    End();
}

void drawPageDebug() {

}

