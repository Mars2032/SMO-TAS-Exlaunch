#include "ImGuiMenu.h"
#include "al/scene/Scene.h"
#include "al/sequence/Sequence.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/OtherUtil.h"
#include "game/GameData/SaveDataAccessFunction.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "game/Player/PlayerActorBase.h"
#include "game/System/GameSystem.h"
#include "game/System/Application.h"
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
static bool isShowRenderer = false;
static bool isShowCollider = false;
static bool isShowAreaObj = false;
static bool isShowTASOptions = false;
static int scenario = -1;

void drawPageMain() {
    if (Begin("Main")) {
        SetWindowFontScale(1.25f);
        Text("Welcome to the SMO-TAS Mod!\n");
        if (Button("Options"))
            isShowOptions = true;
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
    drawPageOptions();
}

void drawPageAbout() {
    if (!isShowAbout) return;
    if (Begin("About", &isShowAbout, ImGuiWindowFlags_NoResize)) {
        SetWindowFontScale(1.25f);
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

void drawPageOptions() {
    if (!isShowOptions) return;
    if (Begin("Options", &isShowOptions)) {
        SetWindowFontScale(1.25f);
        al::Scene* scene = GameSystemFunction::getGameSystem()->mSequence->mCurrentScene;
        if (scene) {
            GameDataHolderAccessor accessor(scene);
            if (Button("Save Options"))
                SaveDataAccessFunction::startSaveDataWriteSync(accessor.mData);
        } else {
            Text("Cannot save, scene is null.");
        }
        if (BeginTabBar("Options")) {
            auto* settings = Settings::instance();
            if (BeginTabItem("General")) {
                Checkbox("Show Actor Positions", &settings->isShowTrans);
                Checkbox("Show Camera Target", &settings->isShowCameraTargetTrans);
                Checkbox("Make Moons Re-Collectable", &settings->isEnableMoonRefresh);
                Checkbox("Make Moons Always Collectable", &settings->isEnableGreyMoonRefresh);
                Checkbox("Auto-Saving", &settings->isEnableAutosave);
                Checkbox("Make Cutscenes Always Skippable", &settings->isSkipCutscenes);

                const char* previewValue = Settings::mPatternEntries[settings->mMofumofuPatternIndex].type;
                SetNextItemWidth(150.f);
                if (BeginCombo("Mechawiggler Patterns", previewValue)) {
                    for (int i = 0; i < IM_ARRAYSIZE(Settings::mPatternEntries); i++) {
                        const bool isSelected = (settings->mMofumofuPatternIndex == i);
                        if (Selectable(Settings::mPatternEntries[i].type, isSelected, 0, ImVec2(150.f,0.f)))
                            settings->mMofumofuPatternIndex = i;
                        if (isSelected)
                            SetItemDefaultFocus();
                    }
                    EndCombo();
                }
                SetNextItemWidth(150.f);
                if (SliderInt("Random Seed", (int*)&settings->mRandomSeed, -1, INT32_MAX/2)) {
                    sead::GlobalRandom::instance()->init(settings->mRandomSeed);
                }
                al::Sequence* curSequence = GameSystemFunction::getGameSystem()->mSequence;
                if (curSequence && al::isEqualString(curSequence->getName().cstr(), "HakoniwaSequence")) {
                    auto gameSeq = (HakoniwaSequence *) curSequence;
                    auto curScene = gameSeq->curScene;
                    WorldResourceLoader* loader = gameSeq->mResourceLoader;
                    bool isInGame = curScene && curScene->mIsAlive && curScene == static_cast<StageScene*>(curScene);
                    if (isInGame) {
                        auto* stageScene = static_cast<StageScene*>(curScene);
                        Text("Scene Name: %s", stageScene->seedText);
                        PlayerActorBase *playerBase = rs::getPlayerActor(stageScene);
                        if (ImGui::Button("Kill Mario")) {
                            PlayerHelper::killPlayer(playerBase);
                        }
                    }
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
                }
                EndTabItem();
            }

            if (BeginTabItem("HitSensor")) {
                Checkbox("Show All HitSensors", &settings->isShowSensors);
                Checkbox("Show Eye Sensors", &settings->isShowEyes);
                Checkbox("Show Attack Sensors", &settings->isShowAttack);
                Checkbox("Show Npc Sensors", &settings->isShowNpc);
                Checkbox("Show Bindable Sensors", &settings->isShowBindable);
                Checkbox("Show EnemyBody Sensors", &settings->isShowEnemyBody);
                Checkbox("Show MapObj Sensors", &settings->isShowMapObj);
                Checkbox("Show Player Sensors", &settings->isShowPlayerAll);
                Checkbox("Show Other Sensors", &settings->isShowOther);
                SetNextItemWidth(150.f);
                SliderFloat("Sensor Alpha", &settings->mSensorAlpha, 0.0f, 1.0f, "%.1f");
                EndTabItem();
            }

            if (BeginTabItem("Collider")) {
                Text("No collider options available at this time.");
                EndTabItem();
            }

            if(BeginTabItem("AreaObj")) {
                Text("No AreaObj options available at this time.");
                EndTabItem();

            }
            EndTabBar();
        }

    }
    End();
}



static constexpr int frameCount = 120;
extern float framerates[frameCount];
extern int frameIndex;

static constexpr float infoWidth = 400.f;

void drawPageInfo() {
    if (!isShowInfo) return;
    if (Begin("Info", &isShowInfo)) {
        SetWindowFontScale(1.25f);
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

            Text("FPS: %.2f", framerates[frameIndex]);
            PlotLines("FPS",framerates, frameCount, frameIndex, nullptr, 0.f, 65.f, ImVec2(150.f,40.f));

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
                    char* name = abi::__cxa_demangle(typeid(*playerBase).name(), nullptr, nullptr, &status);
                    Text("Player Name: %s", name);
                    SetNextItemWidth(infoWidth);
                    InputFloat3("Position", (float*)&playerTrans, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    SetNextItemWidth(infoWidth);
                    InputFloat3("Velocity", (float*)&playerVel, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    SetNextItemWidth(infoWidth);
                    InputFloat("Total Speed", &playerSpeed, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    SetNextItemWidth(infoWidth);
                    InputFloat("Horizontal Speed", &playerSpeedH, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    SetNextItemWidth(infoWidth);
                    InputFloat4("Quat Rotation", (float*)&playerQuat, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    SetNextItemWidth(infoWidth);
                    InputFloat3("Euler Angles", (float*)&playerRPY, "%.3f", ImGuiInputTextFlags_ReadOnly);
                    al::NerveKeeper* playerNerveKeeper = playerBase->getNerveKeeper();
                    if (playerNerveKeeper) {
                        al::Nerve* playerNerve = playerNerveKeeper->getCurrentNerve();
                        char* name2 = abi::__cxa_demangle(typeid(*playerNerve).name(), nullptr, nullptr, &status);
                        Text("Current Nerve: %s", name2 + 23 + strlen(name) + 3);
                        free(name);
                        free(name2);
                        if (playerNerveKeeper->mStateCtrl) {
                            al::State* state = playerNerveKeeper->mStateCtrl->findStateInfo(playerNerve);
                            if (state) {
                                name = abi::__cxa_demangle(typeid(*state->mStateBase).name(), nullptr, nullptr, &status);
                                Text("Current State: %s", name);
                                al::Nerve* stateNerve = state->mStateBase->getNerveKeeper()->getCurrentNerve();
                                name2 = abi::__cxa_demangle(typeid(*stateNerve).name(), nullptr, nullptr, &status);
                                Text("State Nerve: %s", name2 + 23 + strlen(name) + 3);
                                free(name);
                                free(name2);
                            }
                        }
                    } else {
                        Text("NerveKeeper is null.");
                        free(name);
                    }

                    if (CollapsingHeader("Cappy Info")) {
                        if (!al::isEqualString(typeid(*playerBase).name(), typeid(PlayerActorHakoniwa).name()))  {
                            Text("No HackCap in stage.\n");
                        } else {
                            auto* player = static_cast<PlayerActorHakoniwa*>(playerBase);
                            HackCap* cap = player->mHackCap;
                            name = abi::__cxa_demangle(typeid(*cap).name(), nullptr, nullptr, &status);
                            sead::Vector3f capTrans = al::getTrans(cap);
                            sead::Vector3f capVel = al::getVelocity(cap);
                            float capSpeed = al::calcSpeed(cap);
                            float capSpeedH = al::calcSpeedH(cap);
                            sead::Vector3f capRPY = sead::Vector3f::zero;
                            sead::Quatf capQuat = al::getQuat(cap);
                            al::calcQuatRotateDegree(&capRPY, capQuat);
                            Text("Cappy Name: %s", name);
                            SetNextItemWidth(infoWidth);
                            InputFloat3("Position", (float*)&capTrans, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            SetNextItemWidth(infoWidth);
                            InputFloat3("Velocity", (float*)&capVel, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            SetNextItemWidth(infoWidth);
                            InputFloat("Total Speed", &capSpeed, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            SetNextItemWidth(infoWidth);
                            InputFloat("Horizontal Speed", &capSpeedH, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            SetNextItemWidth(infoWidth);
                            InputFloat4("Quat Rotation", (float*)&capQuat, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            SetNextItemWidth(infoWidth);
                            InputFloat3("Euler Angles", (float*)&capRPY, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            al::NerveKeeper* capNerveKeeper = cap->getNerveKeeper();
                            if (capNerveKeeper) {
                                al::Nerve* capNerve = capNerveKeeper->getCurrentNerve();
                                char* name2 = abi::__cxa_demangle(typeid(*capNerve).name(), nullptr, nullptr, &status);
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

        if (CollapsingHeader("Camera Info")) {
            if (scene) {
                al::CameraDirector* director = scene->getCameraDirector();
                if (director) {
                    al::CameraPoseUpdater* updater = director->getPoseUpdater(0);
                    if (updater && updater->mTicket) {
                        al::CameraPoser* cam = updater->mTicket->mPoser;
                        if (cam) {
                            Text("Current Camera: %s", cam->mPoserName);
                            int status;
                            char* name = abi::__cxa_demangle(typeid(*cam).name(), nullptr, nullptr, &status);
                            Text("Camera Type: %s", name);
                            free(name);
                            SetNextItemWidth(infoWidth);
                            InputFloat3("Position", (float*)&cam->mPosition, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            SetNextItemWidth(infoWidth);
                            InputFloat3("Target Pos", (float*)&cam->mTargetTrans, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            SetNextItemWidth(infoWidth);
                            InputFloat3("Up-Dir", (float*)&cam->mCameraUp, "%.3f", ImGuiInputTextFlags_ReadOnly);
                            if(CollapsingHeader("Camera View Matrix")) {
                                InputFloat4("Matrix 'X'", (float*)&cam->mViewMtx.m[0], "%.3f", ImGuiInputTextFlags_ReadOnly);
                                InputFloat4("Matrix 'Y'", (float*)&cam->mViewMtx.m[1], "%.3f", ImGuiInputTextFlags_ReadOnly);
                                InputFloat4("Matrix 'Z'", (float*)&cam->mViewMtx.m[2], "%.3f", ImGuiInputTextFlags_ReadOnly);
                            }
                            InputFloat("FOV-Y", &cam->mFovyDegree, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);

                        } else {
                            Text("Camera is null.");
                        }
                    }
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
        SetWindowFontScale(1.25f);
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

