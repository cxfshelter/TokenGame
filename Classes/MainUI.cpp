//
//  MainUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MainUI.h"
#include "ShopScene.h"
#include "RandomEvent.h"
#include "MakeBuildUI.h"
#include "BattlePubScene.h"
#include "ProductionScene.h"
#include "MakeBuildScene.h"
#include "MakeItemScene.h"
#include "SettingScene.h"
#include "MapScene.h"
#include "TraingHeroScene.h"
#include "AchievementSystem.h"
#include "SimpleAudioEngine.h"
#include "DiamondBuyScene.h"
#include "Network.h"

MainUI* MainUI::sm_MainUI = nullptr;


MainUI* MainUI::getInstance()
{
    if (sm_MainUI == nullptr)
    {
        sm_MainUI = new (std::nothrow) MainUI();
        if (sm_MainUI->Init()) {}
    }
    return sm_MainUI;
}

bool MainUI::Init()
{
    UserEventEntity eventEnity;
    eventEnity.m_UserEventType = UserEventType::PlotTxtLoadFinish;
    eventEnity.m_CallBack = [=](const int noId, const Value& value)
    {
        if (value.asBool()) {
            SlowClear();
        }
    };
    UserEventContainer::getInstance()->Register(eventEnity);
    
    return true;
}


void MainUI::ReleaseInstance()
{
    delete sm_MainUI;
    sm_MainUI = nullptr;
}

void MainUI::UnLoad()
{
    MoveText::getInstance()->ClearTextData();
    ProductionItem::getInstance()->UnSchedule();
    m_RootWidget->unschedule(m_RandomEventName);
    m_RootWidget->unschedule(m_ProgressBar->GetScheduleName());
    delete m_ProgressBar;
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    UIBase::UnLoad();
    
}

void MainUI::LoadFinish()
{
    SetButtonEvent();
    CheckRandomEventTime();
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    ProductionItem::getInstance()->SetSchedule(m_RootWidget);
    //CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("res/Sound/musicMain.mp3");
    //CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("musicMain.mp3");
}

void MainUI::UpdateUI()
{
    ShowButton();
    PlayFirstTimePlot();
    checkTimeWithServer();
    UpdateStorehouse();
    UIUtils::getInstance()->checkShowLeadHand(m_RootWidget);
}

//加载 mainUi 时检查随机时间是否已到
void MainUI::CheckRandomEventTime()
{
    //当前时间的时间戳
    time_t timeStamp = time(nullptr); auto nowTimeStamp = (int)timeStamp;
    
    auto lastRandomEventTimeStamp = UserData::getInstance()->GetUserData(UserDataEnum::RandomEventTime)->asInt();
    
    AddRandomEventSchedule( m_RandomEventTimeInterval - (nowTimeStamp - lastRandomEventTimeStamp) );
}

//增加 随机事件调度器
void MainUI::AddRandomEventSchedule(int intervalTime)
{
    if (intervalTime <= 0)
    {
        CheckRandomEvent();
    }else
    {
        m_RootWidget->scheduleOnce([this](float f)
           {
               
               m_RootWidget->unschedule(m_RandomEventName);
               
               CheckRandomEvent();
               
           }, intervalTime, m_RandomEventName);
        
    }
    
}

void MainUI::CheckRandomEvent()
{
    bool isRandomEvent = false;
    if(RandomEngine::getInstance()->GetDefaultRandomValue(1, 2) == 1)
    {
        int eventId = 0;
        int itemId = 0;
        int randomItemNum = 0;
        int randomGetItemNum = 0;
        if(RandomEvent::getInstance()->GetRandomEvent(eventId, itemId, randomItemNum, randomGetItemNum))
        {
            isRandomEvent = true;
            RandomEventDialog(eventId, itemId, randomItemNum, randomGetItemNum);
        }
        
    }else
    {
        int eventId = 0;
        int talentId = 0;
        if (RandomEvent::getInstance()->GetRandomTalent(eventId, talentId))
        {
            isRandomEvent = true;
            RandomTalentDialog(eventId, talentId);
        }
    }
    
    time_t timeStamp = time(nullptr);
    auto intTimeStamp = (int)timeStamp;
    UserData::getInstance()->Update_Value(UserDataEnum::RandomEventTime, Value(intTimeStamp) );
    
    if (isRandomEvent == false) AddRandomEventSchedule( m_RandomEventTimeInterval);
    
}


//随机事件对话框
void MainUI::RandomEventDialog(int eventId, int itemId, int randomItemNum, int randomGetItemNum)
{
    auto layout_RandomDialog = m_RootWidget->getChildByName<Layout*>("Layout_RandomDialog");
    layout_RandomDialog->setVisible(true);
    
    CfgDataRow dataRow(eventId);
    auto text_Desc = layout_RandomDialog->getChildByName<Text*>("Text_Desc");
    auto langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_Desc->setString(langText);
    
    auto text_TalentDesc = layout_RandomDialog->getChildByName<Text*>("Text_TalentDesc");
    text_TalentDesc->setVisible(false);

    
    auto btn_Buy = layout_RandomDialog->getChildByName<Button*>("Btn_Buy");
    
    auto text_Text = btn_Buy->getChildByName<Text*>("Text_Text");
    
    langId = dataRow.GetValue(CfgField::Choice1Id)->asInt();
    
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_Text->setString(langText);
    
    auto text_GoldPrice = btn_Buy->getChildByName<Text*>("Text_NameNum");
    CfgDataRow itemDataRow(itemId);
    langId = itemDataRow.GetValue(CfgField::Name)->asInt();
    langText = MultiLanguage::getInstance()->GetText(langId);
    langText = StringUtils::format("(%s:%d)", langText.c_str(), randomItemNum);
    text_GoldPrice->setString(langText);
    
    btn_Buy->addClickEventListener([=](Ref* sender)
       {
           //大于零
           if(randomGetItemNum > 0)
           {
               UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, itemId, randomGetItemNum);
               
               CfgDataRow dataRow(eventId);
               auto langId = dataRow.GetValue(CfgField::Goal)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               
               CfgDataRow itemDataRow(itemId);
               langId = itemDataRow.GetValue(CfgField::Name)->asInt();
               langText += " " + MultiLanguage::getInstance()->GetText(langId);
               langText += StringUtils::format("x%d", randomGetItemNum);
               
               MsgListNode::getInstance()->NewMsg(langText);
               
               if (randomGetItemNum > randomItemNum)
               {
                   AchievementSystem::getInstance()->CheckAchievement(UserEventType::GambleVictory, 1);
               }
               
           }else
           {
            
               CfgDataRow dataRow(eventId);
               auto langId = dataRow.GetValue(CfgField::Lose)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               
               CfgDataRow itemDataRow(itemId);
               langId = itemDataRow.GetValue(CfgField::Name)->asInt();
               langText += " " + MultiLanguage::getInstance()->GetText(langId);
               langText += StringUtils::format("x%d", randomItemNum);
               
               MsgListNode::getInstance()->NewMsg(langText);
           }
           layout_RandomDialog->setVisible(false);
           AddRandomEventSchedule(m_RandomEventTimeInterval);
           checkSlideEvent();
       });
    
    
    auto btn_Cancel = layout_RandomDialog->getChildByName<Button*>("Btn_Cancel");
    langId = dataRow.GetValue(CfgField::Choice2Id)->asInt();
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Cancel->setTitleText(langText);
    
    btn_Cancel->addClickEventListener([=](Ref* sender)
      {
          layout_RandomDialog->setVisible(false);
          AddRandomEventSchedule(m_RandomEventTimeInterval);
          checkSlideEvent();
      });
    
}

// 首次进入则产生剧情
void MainUI::PlayFirstTimePlot()
{
//    string userDataDir = FileUtils::getInstance()->getWritablePath()+"AppData";
//    string userDataPath = userDataDir + "/UserBaseData.txt";
//    ValueMap userDataVm = FileUtils::getInstance()->getValueMapFromFile(userDataPath);
//    if( !(userDataVm.empty() || userDataVm.size() == 0) )
//    {
//        // 存在存档，非首次进入
//        return;
//    }
    int buildId = (int)CfgIdEnum::StorehouseId;
    auto buildVal = UserData::getInstance()->Get_MVI_Value(UserDataEnum::MakeBuild, buildId);
    if( buildVal != Value::Null )
    {
        // 存在仓库，非首次进入
        return;
    }
    
    auto layout_black = m_RootWidget->getChildByName("Layout_Black");
    layout_black->setVisible(true);
    auto node_list = m_RootWidget->getChildByName("Node_MsgList");
    auto layout_list = node_list->getChildByName("Layout_MsgList");
    auto img_bg = layout_list->getChildByName("chat_bg_1");
    img_bg->setOpacity(0);
    
    auto langId = (int)LanguageIdEnum::PlotFirstTime;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    MsgListNode::getInstance()->addPlotMsg(langText, m_RootWidget);
    
}

void MainUI::SlowClear()
{
    auto node_list = m_RootWidget->getChildByName("Node_MsgList");
    auto layout_list = node_list->getChildByName("Layout_MsgList");
    auto img_bg = layout_list->getChildByName("chat_bg_1");
    
    auto layout_black = m_RootWidget->getChildByName("Layout_Black");
    layout_black->schedule(
                       [=](float dt)
                       {
                           if (m_opacity <= 20) {
                               m_opacity = m_opacity2 = 255;
                               layout_black->setOpacity(m_opacity);
                               layout_black->setVisible(false);
                               layout_black->unschedule("FirstTimePlot");
                               
                               img_bg->setOpacity(255);
                           }
                           else
                           {
                               m_opacity -= 15;
                               m_opacity2 += 15;
                               layout_black->setOpacity(m_opacity);
                               img_bg->setOpacity(m_opacity2);
                           }
                       }, 0.5, CC_REPEAT_FOREVER, 3, "FirstTimePlot");
    
}

void MainUI::checkTimeWithServer()
{
    int firstLoginTimeStamp = UserData::getInstance()->GetUserData(UserDataEnum::FirstLoginSeconds)->asInt();
    Network::getInstance()->GetServerTime(
                                          [=](int timeStamp)
                                          {
                                              // 可领取的对应登陆天数（1~7）
                                              int awardDay = 0;
                                              if (firstLoginTimeStamp == 0) {
                                                  // 写入文件
                                                  UserData::getInstance()->Update_Value(UserDataEnum::FirstLoginSeconds, Value(timeStamp));
                                                  awardDay = 1;
                                              }
                                              else {
                                                  // 默认直接去掉小数点
                                                  awardDay = (timeStamp - firstLoginTimeStamp) / 86400 + 1;
                                              }
                                              
                                              showLoginAward(awardDay);
                                              
                                          });
    
    
}

/*  param1: 1 - 登陆当天, 2 - 首日登陆第二天
 */
void MainUI::showLoginAward(int loginDay)
{
    // 检测loginDay是否有被领取
    auto layout_loginaward = m_RootWidget->getChildByName<Layout *>("Layout_LoginAward");
    auto loginDayStateVector = UserData::getInstance()->GetUserData(UserDataEnum::LoginAwardState)->asValueVector();
    // true: 被领取，false: 未领取
    bool loginDayState = (loginDayStateVector[loginDay - 1].asInt() != 0);
    
    if (loginDay < 1 || loginDay > 7 || loginDayState) {
        // 当天不可领取，隐藏登陆奖励界面
        layout_loginaward->setVisible(false);
        return;
    }
    
    layout_loginaward->setVisible(true);
    for (int i = 1; i < loginDay; ++i) {
        auto buttonName = StringUtils::format("Button_Day%d", i);
        auto btn = layout_loginaward->getChildByName<ui::Button *>(buttonName);
        btn->setBright(false);
    }
    auto curBtnName = StringUtils::format("Button_Day%d", loginDay);
    auto curBtn = layout_loginaward->getChildByName<ui::Button *>(curBtnName);
    curBtn->setBright(true);
    
}

//随机天赋对话框
void MainUI::RandomTalentDialog(int eventId, int talentId)
{
    auto layout_RandomDialog = m_RootWidget->getChildByName<Layout*>("Layout_RandomDialog");
    layout_RandomDialog->setVisible(true);
    
    CfgDataRow eventIdDataRow(eventId);

    auto text_Desc = layout_RandomDialog->getChildByName<Text*>("Text_Desc");
    
    auto langId = eventIdDataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_Desc->setString(langText);
    
    auto text_TalentDesc = layout_RandomDialog->getChildByName<Text*>("Text_TalentDesc");
    CfgDataRow talentDataRow(talentId);
    
    langId = talentDataRow.GetValue(CfgField::Details)->asInt();
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_TalentDesc->setString(langText); text_TalentDesc->setVisible(true);
    
    auto btn_Buy = layout_RandomDialog->getChildByName<Button*>("Btn_Buy");
    
    auto text_Text = btn_Buy->getChildByName<Text*>("Text_Text");
    langId = (int)LanguageIdEnum::StudyLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    
    langId = talentDataRow.GetValue(CfgField::Name)->asInt();
    langText += MultiLanguage::getInstance()->GetText(langId);
    text_Text->setString(langText);
    
    
    auto text_GoldPrice = btn_Buy->getChildByName<Text*>("Text_NameNum");
    
    auto vmi_Price = &eventIdDataRow.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap();
    
    auto itemId = vmi_Price->begin()->first;
    auto itemNum = vmi_Price->begin()->second.asInt();
    CfgDataRow itemDataRow(itemId);
    
    langId = itemDataRow.GetValue(CfgField::Name)->asInt();
    langText = MultiLanguage::getInstance()->GetText(langId);
    langText = StringUtils::format("(%s:%d)",langText.c_str(), itemNum);
    
    text_GoldPrice->setString(langText);
    
    btn_Buy->addClickEventListener([=](Ref* sender)
       {
           vector<pair<int, int> > lackData;
           CfgDataRow eventIdDataRow(eventId);
           auto vmi_Price = &eventIdDataRow.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap();
           bool delResutl = UserData::getInstance()->DeleteStorehouseData(vmi_Price, &lackData);
           if (delResutl)
           {
               UserData::getInstance()->Add_VI_Fun(UserDataEnum::TalentList, talentId);
               
               CfgDataRow talentDataRow(talentId);
               auto langId = talentDataRow.GetValue(CfgField::Schedule)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               MsgListNode::getInstance()->NewMsg( langText );
               
               AchievementSystem::getInstance()->CheckAchievement(UserEventType::StudyTalent, talentId);
               
           }else
           {
               MoveText::getInstance()->LackResText(m_RootWidget, &lackData);
            }
           layout_RandomDialog->setVisible(false);
           AddRandomEventSchedule(m_RandomEventTimeInterval);
           checkSlideEvent();
       });
    
    
    auto btn_Cancel = layout_RandomDialog->getChildByName<Button*>("Btn_Cancel");
    langId = (int)LanguageIdEnum::NotInterest;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Cancel->setTitleText(langText);
    btn_Cancel->addClickEventListener([=](Ref* sender)
      { 
          layout_RandomDialog->setVisible(false);
          AddRandomEventSchedule(m_RandomEventTimeInterval);
          checkSlideEvent();
      });
}

//炼金获取的物品
void MainUI::AlchemyGetItem()
{
    UserEventContainer::getInstance()->Send(UserEventType::Aichemy, 1);
    
    AchievementSystem::getInstance()->CheckAchievement(UserEventType::Aichemy, 1);
    
    auto goldNum = UserData::getInstance()->GetUserData(UserDataEnum::AlchemyGoldNum)->asInt();
    
    // best test
    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::GoldId, goldNum);
    
    UpdateStorehouse(); ShowButton();
    MoveText::getInstance()->GetResText(m_RootWidget, (int)CfgIdEnum::GoldId, goldNum);
    
    SoundUtils::getInstance()->playMusicSound(GOLD_EFFECT, false);
}

void MainUI::AlchemyCallBackEvent()
{
    auto layout_Button = m_RootWidget->getChildByName<Layout*>("Layout_Button");
    auto btn_Alchemy = layout_Button->getChildByName<Button*>("Btn_Alchemy");
    auto lbar_State = btn_Alchemy->getChildByName<LoadingBar*>("LBar_State");
    
    m_ProgressBar = new ProgressBar();
    m_ProgressBar->SetTotalTime(1);
    m_ProgressBar->SetLoadingBar(lbar_State);
    m_ProgressBar->SetScheduleName(m_AlchemyProgressBar);
    m_ProgressBar->SetCallBack([this]()
       {
           m_ProgressBar->Restart();
           m_RootWidget->unschedule(m_AlchemyProgressBar);
       });
}

void MainUI::SetButtonEvent()
{
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    auto layout_Button = m_RootWidget->getChildByName<Layout*>("Layout_Button");
    auto layout_loginAward = m_RootWidget->getChildByName<Layout*>("Layout_LoginAward");
    
    auto btn_Alchemy = (Button*)layout_Button->getChildByName("Btn_Alchemy");
    auto langId = (int)LanguageIdEnum::BtnCollectLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    
    auto text_AlchemyName = btn_Alchemy->getChildByName<Text*>("Text_Name");
    text_AlchemyName->setString(langText);
    
    //auto shName =   layout_Button->getChildByName<<#typename T#>>(<#const std::string &name#>)
    
    AlchemyCallBackEvent();
    btn_Alchemy->addClickEventListener([this](Ref* sender)
    {
        if(m_ProgressBar->GetRemainTime() == m_ProgressBar->GetTotalTime())
        {
            m_RootWidget->schedule([this](float f)
               {
                   m_ProgressBar->Update(f);
                   
               }, m_AlchemyProgressBar);
            
            AlchemyGetItem();
            m_ProgressBar->Update(0.001);
            
        }
    });
    
    auto btn_Left = layout_title->getChildByName<Button*>("Btn_Left");
    btn_Left->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
        auto scene = BattlePubScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    });
    
    auto btn_Right = layout_title->getChildByName<Button*>("Btn_Right");
    btn_Right->addClickEventListener([this](Ref* sender)
     {
         this->UnLoad();
         auto scene = ProductionScene::createScene();
         Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
     });
    
    auto btn_MakeBuild = layout_Button->getChildByName<Button*>("Btn_MakeBuild");
    langId = (int)LanguageIdEnum::BtnMakeBuildLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_MakeBuild->setTitleText(langText);
    
    btn_MakeBuild->addClickEventListener([this](Ref* sender)
     {
         this->UnLoad();
         auto scene = MakeBuildScene::createScene();
         Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
     });
    
    
    auto btn_MakeItem = layout_Button->getChildByName<Button*>("Btn_MakeItem");
    langId = (int)LanguageIdEnum::BtnMakeItemLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_MakeItem->setTitleText(langText);
    btn_MakeItem->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
        auto scene = MakeItemScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    });
    
    auto btn_Shop = layout_Button->getChildByName<Button*>("Btn_Shop");
    langId = (int)LanguageIdEnum::BtnTradeLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Shop->setTitleText(langText);
    
    btn_Shop->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
        auto scene = ShopScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    });
    
    auto btn_TraingHero = layout_Button->getChildByName<Button*>("Btn_TraingHero");
    
    langId = (int)LanguageIdEnum::BtnTrainLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_TraingHero->setTitleText(langText);

    
    btn_TraingHero->addClickEventListener([this](Ref* sender)
      {
          this->UnLoad();
          auto scene = TraingHeroScene::createScene();
          Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
      });
    
    auto btn_setting = layout_Button->getChildByName<Button*>("Btn_Config");
    btn_setting->addClickEventListener(
                                       [this](Ref* sender)
                                       {
                                           this->UnLoad();
                                           auto scene = SettingScene::createScene();
                                           Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                       });
    
    auto btn_award = layout_loginAward->getChildByName<Button *>("Button_Award");
    btn_award->addClickEventListener(
                                     [=](Ref* sender)
                                     {
                                         layout_loginAward->setVisible(false);
                                     });
    
}

void MainUI::ShowButton()
{
    Button* btn;
    ValueMap* vm;
    auto layout_Button = m_RootWidget->getChildByName<Layout*>("Layout_Button");
    
    vm = &UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild)->asValueMap();
    if (!vm->empty())
    {
        btn = layout_Button->getChildByName<Button*>("Btn_MakeBuild");
        btn->setVisible(true);
        
        for (auto iter = vm->begin(); iter != vm->end(); iter++)
        {
            if (iter->second.asInt() == 0)
            {
                btn->getChildByName<Sprite*>("Sprite_New")->setVisible(true);
                break;
            }
        }
        
        auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
        
        auto tempVal = UserData::getInstance()->Get_MVI_Value(vm, (int)CfgIdEnum::StorehouseId);
        if (tempVal != Value::Null && tempVal.asInt() == (int)MakeBuildUI::MakeBuildStatus::MakeFinish)
        {
            layout_Title->getChildByName("Btn_Right")->setVisible(true);
            auto layout_Storehouse = m_RootWidget->getChildByName<Layout*>("Layout_Storehouse");
            layout_Storehouse->setVisible(true);
            
            auto text_Name = layout_Storehouse->getChildByName<Text*>("Text_Name");
            
            CfgDataRow dataRow((int)CfgIdEnum::StorehouseId);
            auto langId = dataRow.GetValue(CfgField::Name)->asInt();
            auto langText = MultiLanguage::getInstance()->GetText(langId);
            text_Name->setString(langText);
            
        }
        
        tempVal = UserData::getInstance()->Get_MVI_Value(vm, (int)CfgIdEnum::BuildPubId);
        if (tempVal != Value::Null && tempVal.asInt() == (int)MakeBuildUI::MakeBuildStatus::MakeFinish)
        {
            layout_Title->getChildByName("Btn_Left")->setVisible(true);
        }
        
        tempVal = UserData::getInstance()->Get_MVI_Value(vm, (int)CfgIdEnum::TrainingCampId);
        if (tempVal != Value::Null && tempVal.asInt() == (int)MakeBuildUI::MakeBuildStatus::MakeFinish)
        {
            btn = layout_Button->getChildByName<Button*>("Btn_TraingHero");
            btn->setVisible(true);
        }
        
        tempVal = UserData::getInstance()->Get_MVI_Value(vm, (int)CfgIdEnum::BuildItlProduceID);
        if (tempVal != Value::Null && tempVal.asInt() == (int)MakeBuildUI::MakeBuildStatus::MakeFinish)
        {
            ProductionItem::getInstance()->setCBIntervalTime(true);
        }
        
    }
    
    vm = &UserData::getInstance()->GetUserData(UserDataEnum::ShopItem)->asValueMap();
    if (!vm->empty())
    {
        btn = layout_Button->getChildByName<Button*>("Btn_GemShop");
        btn->setVisible(true);
        btn->addClickEventListener(
                                   [this](Ref* sender)
                                   {
                                       this->UnLoad();
                                       auto scene = DiamondBuyScene::createScene();
                                       Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                       
                                       RecordUtils::getInstance()->sdkRecord((int)umengEventID::Press_Shop);
                                   });
        
        btn = layout_Button->getChildByName<Button*>("Btn_Shop");
        btn->setVisible(true);
        for (auto iter = vm->begin(); iter != vm->end(); iter++)
        {
            if (iter->second.asBool())
            {
                btn->getChildByName<Sprite*>("Sprite_New")->setVisible(true);
                break;
            }
        }
    }
    
    vm = &UserData::getInstance()->GetUserData(UserDataEnum::MakeItem)->asValueMap();
    if (!vm->empty())
    {
        btn = layout_Button->getChildByName<Button*>("Btn_MakeItem");
        btn->setVisible(true);
        for (auto iter = vm->begin(); iter != vm->end(); iter++)
        {
            if (iter->second.asBool())
            {
                btn->getChildByName<Sprite*>("Sprite_New")->setVisible(true);
                break;
            }
        }
    }
    
    checkSlideEvent();
}

void MainUI::UpdateStorehouse()
{
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    Layout* layout_ItemTemplate = layout_Template->getChildByName<Layout*>("Layout_ItemTemplate");
    
    auto layout_Storehouse = m_RootWidget->getChildByName<Layout*>("Layout_Storehouse");
    
    auto lView_ItemList = layout_Storehouse->getChildByName<ListView*>("LView_ItemList");
    
    
    int langId;
    Layout* layout_Item;
    
    ValueMap* vm_storehouse =  &(UserData::getInstance()->GetUserData(UserDataEnum::Storehouse)->asValueMap());
    
    //每次清空 子控件
    lView_ItemList->removeAllChildren();
    vector<pair<int, Value>> viv_Storehouse;
    UserData::getInstance()->VMSKToVIV_Sort(vm_storehouse, &viv_Storehouse);
    for (auto pair = viv_Storehouse.begin(); pair != viv_Storehouse.end(); pair++)
    {
        
        auto itemId = pair->first;
        auto itemNum = pair->second.asInt();
        
        if (itemNum == 0)  continue;
        
        CfgDataRow itemRow(itemId);
        if (itemRow.GetValue(CfgField::HideType)->asInt() == 1) {
            continue;
        }
        
        langId = itemRow.GetValue(CfgField::Name)->asInt();
        
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        
        layout_Item = (Layout*)layout_ItemTemplate->clone();
        lView_ItemList->addChild(layout_Item);
        
        auto str = StringUtils::format("%s", langText.c_str());
        layout_Item->getChildByName<Text*>("Text_Name")->setString(str.c_str());
        
        layout_Item->getChildByName<Text*>("Text_Num")->setString(StringUtils::format("%d",itemNum));
        
    }
    lView_ItemList->refreshView();

}

void MainUI::checkSlideEvent()
{
    auto layout_RandomDialog = m_RootWidget->getChildByName<Layout*>("Layout_RandomDialog");
    bool isVisible = layout_RandomDialog->isVisible();
    if (isVisible) {
        UIUtils::getInstance()->addSlideEvent(m_RootWidget, nullptr, nullptr);
    }
    else
    {
        UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                              [=]()
                                              {
                                                  auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
                                                  if(layout_Title->getChildByName<cocos2d::ui::Button *>("Btn_Left")->isVisible())
                                                  {
                                                      this->UnLoad();
                                                      auto scene = BattlePubScene::createScene();
                                                      Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                                  }
                                              },
                                              [=]()
                                              {
                                                  auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
                                                  if(layout_Title->getChildByName<cocos2d::ui::Button *>("Btn_Right")->isVisible())
                                                  {
                                                      this->UnLoad();
                                                      auto scene = ProductionScene::createScene();
                                                      Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
                                                  }
                                              });
    }
    
}

