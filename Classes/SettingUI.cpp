//
//  SettingUI.cpp
//  TokenGame
//
//  Created by best on 15/5/22.
//
//

#include "SettingUI.h"
#include "MainScene.h"
#include "LoadScene.h"
#include "UserEventCallBack.h"
#include "AchievementSystem.h"
#include "UnLockSystem.h"
#include "BattleData.h"

#include "MapUI.h"
#include "MainUI.h"
#include "MakeBuildUI.h"
#include "SimpleAudioEngine.h"

const char * USER_SAVE_FILE1 = "1.txt";
const char * USER_SAVE_FILE2 = "2.txt";
const char * USER_SAVE_FILE3 = "3.txt";

SettingUI* SettingUI::sm_Instance = nullptr;

SettingUI* SettingUI::getInstance()
{
    if (sm_Instance == nullptr)
    {
        sm_Instance = new (std::nothrow) SettingUI();
        if (sm_Instance->Init()) {}
    }
    return sm_Instance;
}

bool SettingUI::Init()
{
    return true;
}

void SettingUI::UnLoad()
{
    UIBase::UnLoad();
}

void SettingUI::UpdateUI()
{
    
}

void SettingUI::LoadFinish()
{
    auto layout_btns = m_RootWidget->getChildByName<Layout *>("Layout_Btns");
    auto layout_title = m_RootWidget->getChildByName<Layout *>("Layout_Title");
    auto layout_other = layout_btns->getChildByName<Layout *>("Layout_other");
    
    auto btn_restart = layout_btns->getChildByName<Button *>("Btn_Restart");
    btn_restart->addClickEventListener(
                                       [this](Ref* sender)
                                       {
                                           this->showConfirmDialog(true);
                                           
                                       });
    
    auto btn_switch = layout_btns->getChildByName<Button *>("Btn_SwithFile");
    btn_switch->addClickEventListener(
                                      [this](Ref* sender)
                                      {
                                          this->showSwitchFileLayout();
                                          
                                      });
    
    auto btn_left = layout_title->getChildByName<Button *>("Btn_Left");
    btn_left->addClickEventListener(
                                    [this](Ref* sender)
                                    {
                                        this->UnLoad();
                                        auto scene = MainScene::createScene();
                                        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                        
                                    });
    
    auto btn_BgMusic = layout_other->getChildByName<Button *>("Btn_SetMusic");
    btn_BgMusic->addClickEventListener(
                                    [this](Ref* sender)
                                    {
                                        bool isOpen = SettingData::getInstance()->GetSettingData(SettingDataEnum::BgMusicIsOpen)->asBool();
                                        
                                        saveSettingData(SettingDataEnum::BgMusicIsOpen, Value(!isOpen));
                                        Button * btn = (Button *)sender;
                                        btn->setBright(!isOpen);
                                        bool isPlay = CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
                                        
                                        if (!isOpen) {
                                            if (!isPlay) {
                                                CocosDenshion::SimpleAudioEngine::getInstance()->rewindBackgroundMusic();
                                            }
                                        }
                                        else {
                                            if (isPlay) {
                                                CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
                                            }
                                        }
                                    });
    
    auto btn_sound = layout_other->getChildByName<Button *>("Btn_SetSound");
    btn_sound->addClickEventListener(
                                     [this](Ref* sender)
                                     {
                                         bool isOpen = SettingData::getInstance()->GetSettingData(SettingDataEnum::SoundEffectIsOpen)->asBool();
                                         saveSettingData(SettingDataEnum::SoundEffectIsOpen, Value(!isOpen));
                                         Button * btn = (Button *)sender;
                                         btn->setBright(!isOpen);
                                         
                                     });
    
    auto btn_battleEffect = layout_other->getChildByName<Button *>("Btn_Effects");
    btn_battleEffect->addClickEventListener(
                                     [this](Ref* sender)
                                     {
                                         bool isOpen = SettingData::getInstance()->GetSettingData(SettingDataEnum::BattleEffectIsOpen)->asBool();
                                         saveSettingData(SettingDataEnum::BattleEffectIsOpen, Value(!isOpen));
                                         Button * btn = (Button *)sender;
                                         btn->setBright(!isOpen);
                                         
                                     });
    
    UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                          [this]()
                                          {
                                              this->UnLoad();
                                              auto scene = MainScene::createScene();
                                              Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                              
                                          }, nullptr);
    
    bool isBgMusic = SettingData::getInstance()->GetSettingData(SettingDataEnum::BgMusicIsOpen)->asBool();
    bool isSound = SettingData::getInstance()->GetSettingData(SettingDataEnum::SoundEffectIsOpen)->asBool();
    bool isBattleEft = SettingData::getInstance()->GetSettingData(SettingDataEnum::BattleEffectIsOpen)->asBool();
    
    btn_BgMusic->setBright(isBgMusic);
    btn_sound->setBright(isSound);
    btn_battleEffect->setBright(isBattleEft);
}

void SettingUI::showConfirmDialog(bool isShow)
{
    auto layout_confirm = m_RootWidget->getChildByName<Layout *>("Layout_ConfirmRestart");
    layout_confirm->setVisible(isShow);
    if (!isShow) {
        return;
    }
    
    auto btn_confirm = layout_confirm->getChildByName<Button *>("Button_Confirm");
    btn_confirm->addClickEventListener(
                                       [this](Ref* sender)
                                       {
                                           restartGame();
                                           UnLoad();
                                       });
    
    auto btn_cancel = layout_confirm->getChildByName<Button *>("Button_Cancel");
    btn_cancel->addClickEventListener(
                                       [this](Ref* sender)
                                       {
                                           showConfirmDialog(false);
                                       });
}

void SettingUI::restartGame()
{
    string userDataPath = UserData::getInstance()->getUserDataFilePath();
    string userMapPath = UserData::getInstance()->getUserMapFilePath();
    if (FileUtils::getInstance()->isFileExist(userDataPath)) {
        if(FileUtils::getInstance()->removeFile(userDataPath)) {
            FileUtils::getInstance()->removeFile(userMapPath);
            switchGame();
        }
    }
    
}

void SettingUI::switchGame()
{
    UserData::getInstance()->releaseData();
    BattleData::getInstance()->releaseData();
    AchievementSystem::getInstance()->releaseData();
    UnLockSystem::getInstance()->releaseData();
    UserEventContainer::getInstance()->releaseData();
    
    // 凡是注册了事件的界面都要调用强制释放，
    MapUI::getInstance()->ReleaseInstance();
    MainUI::getInstance()->ReleaseInstance();
    MakeBuildUI::getInstance()->ReleaseInstance();
    
    MsgListNode::getInstance()->clearMsg();
    
    UnLoad();
    auto scene = LoadScene::createScene();
    Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    
}

void SettingUI::saveSettingData(SettingDataEnum setType, Value val)
{
    if (val.isNull()) {
        return;
    }
    
    Value * oriVal = SettingData::getInstance()->GetSettingData(setType);
    *oriVal = val;
    
    SettingData::getInstance()->SaveSettingData();
}

void SettingUI::showSwitchFileLayout()
{
    auto layout_switchFile = m_RootWidget->getChildByName<Layout *>("Layout_SwithFile");
    layout_switchFile->setVisible(true);
    
    auto btn_file1 = layout_switchFile->getChildByName<Button *>("Button_File1");
    btn_file1->addClickEventListener(
                                     [this](Ref* sender)
                                     {
                                         this->saveSettingData(SettingDataEnum::CurFileDocName, Value(USER_SAVE_FILE1));
                                         switchGame();
                                     });
    
    auto btn_file2 = layout_switchFile->getChildByName<Button *>("Button_File2");
    btn_file2->addClickEventListener(
                                     [this](Ref* sender)
                                     {
                                         this->saveSettingData(SettingDataEnum::CurFileDocName, Value(USER_SAVE_FILE2));
                                         switchGame();
                                     });
    
    auto btn_file3 = layout_switchFile->getChildByName<Button *>("Button_File3");
    btn_file3->addClickEventListener(
                                     [this](Ref* sender)
                                     {
                                         this->saveSettingData(SettingDataEnum::CurFileDocName, Value(USER_SAVE_FILE3));
                                         switchGame();
                                     });
    
    auto btn = layout_switchFile->getChildByName<Button *>("Button_Cancel");
    btn->addClickEventListener(
                               [layout_switchFile](Ref* sender)
                               {
                                   layout_switchFile->setVisible(false);
                               });
    
}



