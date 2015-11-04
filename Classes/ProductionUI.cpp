//
//  ProductionUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//


#include "MainScene.h"
#include "ProductionItem.h"
#include "ProductionUI.h"
#include "RandomEngine.h"
#include "AchievementSystem.h"


ProductionUI* ProductionUI::sm_ProductionUI = nullptr;

ProductionUI* ProductionUI::getInstance()
{
    if (sm_ProductionUI == nullptr)
    {
        sm_ProductionUI = new (std::nothrow) ProductionUI();
        if (sm_ProductionUI->Init()) {}
    }
    return sm_ProductionUI;
}

bool ProductionUI::Init()
{
    return true;
}

void ProductionUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    ProductionItem::getInstance()->UnSchedule();
    m_RootWidget->unschedule(m_ProgressBar->GetScheduleName()); delete m_ProgressBar;
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    UIBase::UnLoad();
}

void ProductionUI::UpdateUI()
{
    ShowProductionOption(); ShowProductionList();
}

void ProductionUI::UpdateWorkerNum()
{
    auto layout_Rigth = m_RootWidget->getChildByName<Layout*>("Layout_Rigth");
    auto lview_Options = layout_Rigth->getChildByName<ListView*>("LView_Options");
    
    vector<pair<int, Value>> viv;
    auto val_Production = UserData::getInstance()->GetUserData(UserDataEnum::Production);
    auto vm_Production = &val_Production->asValueMap();
    UserData::getInstance()->VMSKToVIV_Sort(vm_Production, &viv);
    for (auto iter = viv.begin(); iter != viv.end(); iter ++)
    {
        auto cfgId = iter->first;
        auto layout_item = lview_Options->getChildByTag(cfgId);
        if (!layout_item) continue;
        
        auto text_Num = layout_item->getChildByName<Text*>("Text_Num");
        text_Num->setString(StringUtils::format("%d", iter->second.asInt()));
    }
    
    auto nowWorkerNum = GetNowWorkerNum();
    auto maxWorkerNum = GetMaxWorkerNum();
    if (maxWorkerNum > 0)
    {
        auto layout_Left = m_RootWidget->getChildByName<Layout*>("Layout_Left");
        layout_Left->getChildByName<Text*>("Text_Worker")->setVisible(true);
        auto text_WorkerNum = layout_Left->getChildByName<Text*>("Text_WorkerNum");
        text_WorkerNum->setVisible(true);
        text_WorkerNum->setString(StringUtils::format("%d/%d", nowWorkerNum, maxWorkerNum));
    }
}

void ProductionUI::ShowProductionOption()
{
    auto val_Production = UserData::getInstance()->GetUserData(UserDataEnum::Production);
    auto vm_Production = &val_Production->asValueMap();
    if (!vm_Production->empty())
    {
        auto layout_Rigth = m_RootWidget->getChildByName<Layout*>("Layout_Rigth");
        layout_Rigth->setVisible(true);
        
        auto lview_Options = layout_Rigth->getChildByName<ListView*>("LView_Options");
        
        auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
        
        auto layout_Option_Template = layout_Template->getChildByName<Layout*>("Layout_Option");
        
        int cfgId, langId;
        string langText;
        
        lview_Options->removeAllChildren();
        vector<pair<int, Value>> viv;
        UserData::getInstance()->VMSKToVIV_Sort(vm_Production, &viv);
        for (auto iter = viv.begin(); iter != viv.end(); iter ++)
        {
            auto layout_option = layout_Option_Template->clone();
            lview_Options->addChild(layout_option);
            
            cfgId = iter->first;
            layout_option->setTag(cfgId);
            
            langId = CfgData::getInstance()->GetValue(iter->first, CfgField::Name)->asInt();
            langText = MultiLanguage::getInstance()->GetText( langId );

            auto btn_Name = layout_option->getChildByName<Button*>("Btn_Name");
            
            btn_Name->setTitleText(langText + "工人");
            btn_Name->addClickEventListener([=](Ref* sender)
                {
                    CfgDataRow dataRow(cfgId);
                    
                    auto langId = dataRow.GetValue(CfgField::Details)->asInt();
                    
                    auto langText = MultiLanguage::getInstance()->GetText(langId);
                    
                    string text = langText + "\n";
                    
                    auto material = dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
                    
                    if (!material.empty())
                    {
                        langId = (int)LanguageIdEnum::ProductionExpend;
                        
                        langText = MultiLanguage::getInstance()->GetText(langId);
                        
                        text += langText;
                        for (auto itemData : material)
                        {
                            CfgDataRow itemDataRow(itemData.first);
                            
                            langId = itemDataRow.GetValue(CfgField::Name)->asInt();
                            langText = MultiLanguage::getInstance()->GetText(langId);
                            
                            text += langText += StringUtils::format(" - %d", itemData.second.asInt());
                        }
                    }
                    text += "\n";
                    
                    langId = (int)LanguageIdEnum::ProductionOutput;
                    
                    langText = MultiLanguage::getInstance()->GetText(langId);
                    
                    
                    auto outputNum = dataRow.GetValue(CfgField::Output)->asInt();
                    
                    langId = dataRow.GetValue(CfgField::Name)->asInt();
                    langText += MultiLanguage::getInstance()->GetText(langId);
                    
                    text += langText += StringUtils::format(" + %d", outputNum);
                    
                    TextExplainNode::getInstance()->UpdateText(m_RootWidget, &text);
                    
                });
            
            auto text_Num = layout_option->getChildByName<Text*>("Text_Num");
            text_Num->setString(StringUtils::format("%d", iter->second.asInt()));
            
            auto btn_Add = layout_option->getChildByName<Button*>("Btn_Add");
            btn_Add->addClickEventListener([this, cfgId](Ref* sender)
             {
                 auto nowNum = ProductionUI::getInstance()->GetNowWorkerNum();
                 auto maxNum = ProductionUI::getInstance()->GetMaxWorkerNum();
                 if (nowNum < maxNum)
                 {
                     UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Production, cfgId, 1, false, false, false);
                 }
                 UpdateWorkerNum();
                 ShowProductionList();
             });
            
            btn_Add->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type)
            {
                Button * btn = (Button *)pSender;
                Node* node = btn->getChildByName("ANode");
                if (node == nullptr) {
                    node = Node::create();
                    node->setName("ANode");
                    btn->addChild(node);
                }
                
                switch (type) {
                    case Widget::TouchEventType::BEGAN:
                        node->schedule(
                           [=](float dt)
                           {
                               node->unschedule("LONG_BTN_BEGAN");
                               node->schedule(
                              [=](float dt)
                              {
                                  auto nowNum = ProductionUI::getInstance()->GetNowWorkerNum();
                                  auto maxNum = ProductionUI::getInstance()->GetMaxWorkerNum();
                                  if (nowNum < maxNum)
                                  {
                                      UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Production, cfgId, 1, false, false, false);
                                  }
                                  UpdateWorkerNum();
                                  ShowProductionList();
                              }, BTN_ADD_TIME, "LONG_BTN_ADD"
                              );
                           }, LONG_BTN_TIME, "LONG_BTN_BEGAN");
                        break;
                    case Widget::TouchEventType::MOVED:
                        break;
                    default:
                        node->unschedule("LONG_BTN_BEGAN");
                        node->unschedule("LONG_BTN_ADD");
                        break;
                }
            });
            
            auto btn_Reduce = layout_option->getChildByName<Button*>("Btn_Reduce");
            btn_Reduce->addClickEventListener([this, cfgId](Ref* sender)
            {
                auto data = UserData::getInstance()->Get_MVI_Value(UserDataEnum::Production, cfgId);
                if ( data != Value::Null && data.asInt() > 0)
                {
                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Production, cfgId, -1, false, false, false);
                }
                UpdateWorkerNum();
                ShowProductionList();
            });
            btn_Reduce->addTouchEventListener([=](Ref* pSender, Widget::TouchEventType type)
            {
                Button * btn = (Button *)pSender;
                Node* node = btn->getChildByName("ANode");
                if (node == nullptr) {
                    node = Node::create();
                    node->setName("ANode");
                    btn->addChild(node);
                }
                
                switch (type) {
                    case Widget::TouchEventType::BEGAN:
                        node->schedule(
                       [=](float dt)
                       {
                           node->unschedule("LONG_BTN_BEGAN");
                           node->schedule(
                          [=](float dt)
                          {
                              auto data = UserData::getInstance()->Get_MVI_Value(UserDataEnum::Production, cfgId);
                              if ( data != Value::Null && data.asInt() > 0)
                              {
                                  UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Production, cfgId, -1, false, false, false);
                              }
                              UpdateWorkerNum();
                              ShowProductionList();
                          }, BTN_ADD_TIME, "LONG_BTN_REDUCE"
                          );
                       }, LONG_BTN_TIME, "LONG_BTN_BEGAN");

                        break;
                        case Widget::TouchEventType::MOVED:
                        break;
                        default:
                        node->unschedule("LONG_BTN_BEGAN");
                        node->unschedule("LONG_BTN_REDUCE");
                        break;
                }
            });
            
        }
        
        lview_Options->refreshView();
    }
    
    auto nowWorkerNum = GetNowWorkerNum();
    auto maxWorkerNum = GetMaxWorkerNum();
    if (maxWorkerNum > 0)
    {
        auto layout_Left = m_RootWidget->getChildByName<Layout*>("Layout_Left");
        layout_Left->getChildByName<Text*>("Text_Worker")->setVisible(true);
        auto text_WorkerNum = layout_Left->getChildByName<Text*>("Text_WorkerNum");
        text_WorkerNum->setVisible(true);
        text_WorkerNum->setString(StringUtils::format("%d/%d", nowWorkerNum, maxWorkerNum));
    }
//    UpdateWorkerNum();
}

void ProductionUI::ShowProductionList()
{
    auto layout_Rigth = m_RootWidget->getChildByName<Layout*>("Layout_Rigth");
    
    auto lview_Item = layout_Rigth->getChildByName<ListView*>("LView_Item");
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Item_Template = layout_Template->getChildByName<Layout*>("Layout_Item");
    

    map<int, int> outItems;
    auto itemId = 0, itemNum = 0;
    lview_Item->removeAllChildren();
    
    ProductionItem::getInstance()->ComputeProductionItemList(&outItems);
    auto iter = outItems.begin();
    while (iter != outItems.end())
    {
        itemId = iter->first;
        itemNum = iter->second;
        
        iter++; if(itemNum == 0) continue;
        
        auto layout_Item = layout_Item_Template->clone();
        lview_Item->addChild(layout_Item);
        
        CfgDataRow dataRow(itemId);
        auto text_Name = layout_Item->getChildByName<Text*>("Text_Name");

        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        text_Name->setString(langText);
        
        auto text_Num = layout_Item->getChildByName<Text*>("Text_Num");
        auto strNum = StringUtils::format("%d", itemNum);
        text_Num->setString(strNum);

    }
    
    lview_Item->refreshView();
    
}

//采集获取的物品
void ProductionUI::CollectGetItem()
{
    UserEventContainer::getInstance()->Send(UserEventType::CollectEvent);
    
    AchievementSystem::getInstance()->CheckAchievement(UserEventType::CollectEvent, 1);
    
    
    auto minNum = UserData::getInstance()->GetUserData(UserDataEnum::CollectMinNum)->asInt();
    auto maxNum = UserData::getInstance()->GetUserData(UserDataEnum::CollectMaxNum)->asInt();
    
    int woodNum = RandomEngine::getInstance()->GetDefaultRandomValue(minNum, maxNum);
    int StoneNum = RandomEngine::getInstance()->GetDefaultRandomValue(minNum, maxNum);
    
    auto val_Storehouse = UserData::getInstance()->GetUserData(UserDataEnum::Storehouse);
    UserData::getInstance()->Update_VMI_Fun(&val_Storehouse->asValueMap(), (int)CfgIdEnum::WoodId, woodNum);
    UserData::getInstance()->Update_VMI_Fun(&val_Storehouse->asValueMap(), (int)CfgIdEnum::StoneId, StoneNum);
    
    MoveText::getInstance()->GetResText(m_RootWidget, (int)CfgIdEnum::WoodId, woodNum);
    
    MoveText::getInstance()->GetResText(m_RootWidget, (int)CfgIdEnum::StoneId, StoneNum);
    
    SoundUtils::getInstance()->playMusicSound(TILIAN_EFFECT, false);
    
    UIUtils::getInstance()->checkShowLeadHand(m_RootWidget);
}

void ProductionUI::CollectCallBackEvent()
{
    auto layout_Left = m_RootWidget->getChildByName<Layout*>("Layout_Left");
    //采集事件
    auto btn_Collect = layout_Left->getChildByName<Button*>("Btn_Collect");

    auto lbar_Status = btn_Collect->getChildByName<LoadingBar*>("LBar_Status");
    
    int nowTime = (int)time(nullptr);
    
    auto collectInvertTime = m_CollectInvertTime;
    if( m_CollectInvertTime - (nowTime - m_LastCollectTime) > 0)
    {
        collectInvertTime = m_CollectInvertTime - (nowTime - m_LastCollectTime);
    }
    m_ProgressBar = new ProgressBar();
    m_ProgressBar->SetTotalTime(m_CollectInvertTime);
    m_ProgressBar->SetRemainTime(collectInvertTime);
    m_ProgressBar->SetLoadingBar(lbar_Status);
    m_ProgressBar->SetScheduleName(m_CollectProgressBar);
    
    m_ProgressBar->SetCallBack([this]()
       {
           m_ProgressBar->Restart();
           m_RootWidget->unschedule(m_CollectProgressBar);
       });
    
    if (collectInvertTime != m_CollectInvertTime)
    {
        m_RootWidget->schedule([this](float f)
           {
               m_ProgressBar->Update(f);
               
           }, m_CollectProgressBar);
    }
}

void ProductionUI::LoadFinish()
{
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");

    auto btn_Left = layout_Title->getChildByName<Button*>("Btn_Left");
    
    btn_Left->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
        auto scene = MainScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    });

    auto layout_Left = m_RootWidget->getChildByName<Layout*>("Layout_Left");
    
    auto layout_Rigth = m_RootWidget->getChildByName<Layout*>("Layout_Rigth");
    
    //采集事件
    auto btn_Collect = layout_Left->getChildByName<Button*>("Btn_Collect");
    
    auto langId = (int)LanguageIdEnum::CollectLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    auto text_Name = btn_Collect->getChildByName<Text*>("Text_Name");
    text_Name->setString(langText);
    
    
    auto text_Text = layout_Rigth->getChildByName<Text*>("Text_Text");
    langId = (int)LanguageIdEnum::NextHarvest;
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_Text->setString(langText);
    
    auto text_Time = layout_Rigth->getChildByName<Text*>("Text_Time");
    
    ProductionItem::getInstance()->SetSchedule(m_RootWidget, text_Time);
    
    CollectCallBackEvent();
    btn_Collect->addClickEventListener([this](Ref* sender)
    {
        if(m_ProgressBar->GetRemainTime() == m_ProgressBar->GetTotalTime())
        {
            CollectGetItem();
            m_LastCollectTime = (int)time(nullptr);
            m_RootWidget->schedule([this](float f)
               {
                   m_ProgressBar->Update(f);
                   
               }, m_CollectProgressBar);
            m_ProgressBar->Update(0.001);
        }
        
    });
    
    UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                          [this]()
                                          {
                                              this->UnLoad();
                                              auto scene = MainScene::createScene();
                                              Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                          }, nullptr);
}


//获取当前正在工作中的工人数量
int ProductionUI::GetNowWorkerNum()
{
    auto result = 0;
    auto val_Production = UserData::getInstance()->GetUserData(UserDataEnum::Production);
    auto vm_Production = &val_Production->asValueMap();
    auto iter = vm_Production->begin();
    while (iter != vm_Production->end())
    {
        result += iter->second.asInt();
        iter++;
    }
    return result;
}

//根据当前民宅 工人数量
int ProductionUI::GetMaxWorkerNum()
{
    int cfgId = 0;
    auto result = 0;
    auto val_makeBuild = UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild);
    auto vm_makeBuild = val_makeBuild->asValueMap();
    for (auto iter = vm_makeBuild.begin(); iter != vm_makeBuild.end(); iter++)
    {
        cfgId = atoi(iter->first.c_str());
        if (cfgId >= 100001001 && cfgId <= 100001040)
        {
            if (cfgId == 100001001)
            {
                cfgId = 0;
                
            }else if(cfgId == 100001040)
            {
                if(iter->second.asInt() == 2)
                {
                    cfgId = 100001040;
                }else
                {
                    cfgId = 100001039;
                }
            }else
            {
                cfgId = cfgId - 1;
            }
            break;
        }
    }
    
    if(cfgId != 0)
    {
        auto data = CfgData::getInstance()->GetValue(cfgId, CfgField::FunctionID_M);
        
        auto iter = data->asIntMultiKeyMap().find(cfgId);
        if(iter != data->asIntMultiKeyMap().end())
        {
            result = iter->second.asInt();
        }
    }
    return result;
}


