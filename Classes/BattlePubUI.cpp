//
//  BattlePubUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MapScene.h"
#include "MainScene.h"
#include "BattlePubUI.h"
#include "TalentScene.h"
#include "AchievementScene.h"
#include "RankScene.h"

BattlePubUI* BattlePubUI::sm_BattlePubUI = nullptr;

BattlePubUI* BattlePubUI::getInstance()
{
    if (sm_BattlePubUI == nullptr)
    {
        sm_BattlePubUI = new (std::nothrow) BattlePubUI();
        if (sm_BattlePubUI->Init()) {}
    }
    return sm_BattlePubUI;
}

bool BattlePubUI::Init()
{
    return true;
}

void BattlePubUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    ProductionItem::getInstance()->UnSchedule();
    UIBase::UnLoad();
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    m_longTextShowNum = 0;
}

void BattlePubUI::UpdateUI()
{
    
    UpdateBackPackHeroNum(); UpdateBackPackText_RanksText();
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    auto layout_Option = layout_Template->getChildByName<Layout*>("Layout_Option");
    
    auto layout_Right = m_RootWidget->getChildByName<Layout*>("Layout_Right");
    auto lview_List = layout_Right->getChildByName<ListView*>("LView_List");
    
    auto vm_Shorehouse = &UserData::getInstance()->GetUserData(UserDataEnum::Storehouse)->asValueMap();
    
    vector<int> sb; //把背包和 仓库的能出征的物品id合并
    for (auto iter = vm_Shorehouse->begin(); iter != vm_Shorehouse->end(); iter++)
    {
        auto cfgId = atoi(iter->first.c_str());
        CfgDataRow dataRow(cfgId);
        
        if (dataRow.GetValue(CfgField::Expedition)->asInt() == 1)
        {
            sb.push_back(cfgId);
        }
    }
    
    auto vm_backPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    for (auto data : *vm_backPack)
    {
        auto cfgId = atoi(data.first.c_str());
        if (cfgId / 1000000 == 205) continue;
        
        if (find(sb.begin(), sb.end(), cfgId) == sb.end())
        {
            CfgDataRow dataRow(cfgId);
            if (dataRow.GetValue(CfgField::Expedition)->asInt() == 1)
            {
                sb.push_back(cfgId);
            }
        }
    }
    
    for (auto iter = sb.begin(); iter != sb.end(); iter++)
    {
        auto cfgId = *iter;
        CfgDataRow dataRow(cfgId);

        auto layout_Item = layout_Option->clone();
        lview_List->addChild(layout_Item);
        
        auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Name->setTitleText(langText.c_str());
        
        btn_Name->addClickEventListener([=](Ref* sender)
           {
               CfgDataRow dataRow(cfgId);
               auto langId = dataRow.GetValue(CfgField::Details)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText);
           });
        
        auto level = dataRow.GetValue(CfgField::Level)->asInt();
        auto layout_Start = layout_Item->getChildByName<Layout*>("Start");
        if(layout_Start != nullptr)
        {
            for (auto i = 1; i <= level; i++)
            {
                auto startName = StringUtils::format("Start_%d", i - 1);
                auto imgStart = layout_Start->getChildByName<ImageView*>(startName);
                if (imgStart != nullptr) imgStart->setVisible(true);
            }
        }
        
        auto backPackNum = 0;
        auto val_BackPack = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, cfgId);
        if (val_BackPack != Value::Null)
        {
            backPackNum = val_BackPack.asInt();
        }
        auto text_Num = layout_Item->getChildByName<Text*>("Text_Num");
        text_Num->setString(StringUtils::format("%d", backPackNum));
        
        auto btn_Add = layout_Item->getChildByName<Button*>("Btn_Add");
        btn_Add->addClickEventListener([=](Ref* sender)
           {
               BtnAddReduceEvent(cfgId, layout_Item, 1);
           });
        
        // 长按监听
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
                           CCLOG("在增加中: %f", dt);
                           BtnAddReduceEvent(cfgId, layout_Item, 1);
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
        
        auto btn_Reduce = layout_Item->getChildByName<Button*>("Btn_Reduce");
        btn_Reduce->addClickEventListener([=](Ref* sender)
          {
              auto backPackNum = UpdateBackPackNum(cfgId, -1);
              if (backPackNum >= 0)
              {
                  UpdateBackPackText_RanksText();
                  text_Num->setString(StringUtils::format("%d", backPackNum));
                  m_longTextShowNum = 0;
              }
          });
        
        btn_Reduce->addTouchEventListener(
          [=](Ref* pSender, Widget::TouchEventType type)
          {
              Button * btn = (Button *)pSender;
              Node * node = btn->getChildByName("BNode");
              if (node == nullptr) {
                  node = Node::create();
                  node->setName("BNode");
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
                                                CCLOG("在增加中: %f", dt);
                                                BtnAddReduceEvent(cfgId, layout_Item, -1);
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
    
    auto vm_LiveHero = &UserData::getInstance()->GetUserData(UserDataEnum::LiveHero)->asValueMap();
    for (auto iter = vm_LiveHero->begin(); iter != vm_LiveHero->end(); iter++)
    {
        auto cfgId = atoi(iter->first.c_str());
        CfgDataRow dataRow(cfgId);
        auto layout_Item = layout_Option->clone();
        lview_List->addChild(layout_Item);
        
        auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Name->setTitleText(langText.c_str());
        
        btn_Name->addClickEventListener([=](Ref* sender)
            {
                CfgDataRow dataRow(cfgId);
                auto langId = dataRow.GetValue(CfgField::Details)->asInt();
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText, cfgId);
            });

        
        auto level = dataRow.GetValue(CfgField::Level)->asInt();
        auto layout_Start = layout_Item->getChildByName<Layout*>("Start");
        for (auto i = 1; i <= level; i++)
        {
            auto startName = StringUtils::format("Start_%d", i - 1);
            auto imgStart = layout_Start->getChildByName<ImageView*>(startName);
            if (imgStart != nullptr) imgStart->setVisible(true);
        }
        
        auto backPackNum = 0;
        auto val_BackPack = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, cfgId);
        if (val_BackPack != Value::Null)
        {
            backPackNum = val_BackPack.asInt();
        }
        auto text_Num = layout_Item->getChildByName<Text*>("Text_Num");
        text_Num->setString(StringUtils::format("%d", backPackNum));
        
        auto btn_Add = layout_Item->getChildByName<Button*>("Btn_Add");
        btn_Add->addClickEventListener([this, cfgId, text_Num](Ref* sender)
           {
               auto backPackNum = UpdateLiveHero(cfgId, 1);
               
               switch (backPackNum)
               {
                   case -1:
                   {
                       CfgDataRow dataRow(cfgId);
                       auto langId =dataRow.GetValue(CfgField::Name)->asInt();
                       auto langText = MultiLanguage::getInstance()->GetText(langId);
                       langId = (int)LanguageIdEnum::NotEnoughLangId;
                       langText += MultiLanguage::getInstance()->GetText(langId);
                       MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
                       
                       break;
                    }
                       
                   case -2:
                   {
                       auto langId = (int)LanguageIdEnum::TeamSpaceNotEnoughId;
                       auto langText = MultiLanguage::getInstance()->GetText(langId);
                       MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
                       break;
                   }
                   default:
                   {
                       UpdateBackPackText_RanksText();
                       text_Num->setString(StringUtils::format("%d", backPackNum));
                       break;

                   }
               }
               
            });

        auto btn_Reduce = layout_Item->getChildByName<Button*>("Btn_Reduce");
        btn_Reduce->addClickEventListener([this, cfgId, text_Num](Ref* sender)
          {
              auto backPackNum = UpdateLiveHero(cfgId, -1);
              if (backPackNum >= 0)
              {
                  UpdateBackPackText_RanksText();
                  text_Num->setString(StringUtils::format("%d", backPackNum));
              }
          });
        
        auto val = UserData::getInstance()->Get_MVI_Value(UserDataEnum::LiveHero, cfgId);
        if (val != Value::Null)
        {
            auto btn_Num = btn_Name->getChildByName<Button*>("Btn_Num");
            btn_Num->setTitleText(StringUtils::format("%d", val.asInt()));
            btn_Num->setVisible(true);
        }
        
        UIUtils::getInstance()->showProperImg(btn_Name, cfgId);
        
    }
    
}

void BattlePubUI::BtnAddReduceEvent(int cfgId, Widget* layoutItem, int addNum)
{
    auto backPackNum = UpdateBackPackNum(cfgId, addNum);
    auto text_Num = layoutItem->getChildByName<Text*>("Text_Num");
    if (!text_Num) {
        return;
    }
    
    switch (backPackNum)
    {
        case -1:
        {
            if (m_longTextShowNum > 0) {
                return;
            }
            
            CfgDataRow dataRow(cfgId);
            auto langId =dataRow.GetValue(CfgField::Name)->asInt();
            auto langText = MultiLanguage::getInstance()->GetText(langId);
            langId = (int)LanguageIdEnum::NotEnoughLangId;
            langText += MultiLanguage::getInstance()->GetText(langId);
            MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
            m_longTextShowNum += 1;
            break;
        }
        case -2:
        {
            if (m_longTextShowNum > 0) {
                return;
            }
            
            auto langId = (int)LanguageIdEnum::BakcPackSpaceNotEnoughId;
            auto langText = MultiLanguage::getInstance()->GetText(langId);
            MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
            m_longTextShowNum += 1;
            break;
        }
        default:
        {
            UpdateBackPackText_RanksText();
            text_Num->setString(StringUtils::format("%d", backPackNum));
            break;
        }
    }
}

//处理 英雄升级后 背包里的英雄数量要调整
void BattlePubUI::UpdateBackPackHeroNum()
{
    vector<pair<int, int>> heroVec;
    GetBackPackHero(&heroVec);
    auto vm_LiveHero = &UserData::getInstance()->GetUserData(UserDataEnum::LiveHero)->asValueMap();
    
    auto heroId = 0;
    auto heroNum = 0;
    for(auto heroData : heroVec)
    {
        heroId = heroData.first;
        heroNum = heroData.second;
        string heroStrId = StringUtils::format("%d", heroId);
        auto liveHero = vm_LiveHero->find(heroStrId);
        if (liveHero == vm_LiveHero->end())
        {
            UserData::getInstance()->Delete_VMI_Fun(UserDataEnum::BackPack, heroId);
        }else
        {
            if (liveHero->second.asInt() < heroNum)
            {
                UserData::getInstance()->Update_VMI_Fun(
                UserDataEnum::BackPack, heroId, liveHero->second.asInt(), true);
            }
        }
    }
    
    
}

//更新英雄数量 反回 -1 英雄不够 -2 队伍已满
int BattlePubUI::UpdateLiveHero(int cfgId, int num)
{
    
    int liveHeroNum = 0;
    int backPackNowHeroNum = 0;
    int backPackTotalHeroNum = 0;
    
    bool isEnough = true;
    
    //获取训练营里的当前英雄数量
    auto liveHeroVal = UserData::getInstance()->Get_MVI_Value(UserDataEnum::LiveHero, cfgId);
    if (liveHeroVal != Value::Null) liveHeroNum = liveHeroVal.asInt();

    //背包中当前英雄的数量
    auto backPackNowHeroVal = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, cfgId);
    if (backPackNowHeroVal != Value::Null) backPackNowHeroNum = backPackNowHeroVal.asInt();

    
    //背包中的总英雄数量
    backPackTotalHeroNum = GetBackPackHeroNum();
    
    backPackNowHeroNum += num;
    backPackTotalHeroNum += num;
    
    if (backPackNowHeroNum > liveHeroNum || backPackNowHeroNum < 0)
    {
        backPackNowHeroNum = -1;
        isEnough = false;
    }
    
    auto ranksMaxNum = UserData::getInstance()->GetUserData(UserDataEnum::RanksMaxNum)->asInt();
    if (isEnough && backPackTotalHeroNum > ranksMaxNum )
    {
        backPackNowHeroNum = -2;
        isEnough = false;
    }
    if (isEnough)
    {
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::BackPack, cfgId, num);
    }
    return backPackNowHeroNum;
}

//获取背包中的英雄数量
int BattlePubUI::GetBackPackHeroNum()
{
    auto heroNum = 0;
    vector<pair<int, int>> heroVec;
    GetBackPackHero(&heroVec);
    for (auto data : heroVec)
    {
        heroNum += data.second;
    }
    return heroNum;
}
//获取背包中的所有英雄
void BattlePubUI::GetBackPackHero(vector<pair<int, int>>* heroVec)
{
    heroVec->clear();
    auto vm_BackPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    for (auto iter = vm_BackPack->begin(); iter != vm_BackPack->end(); iter++)
    {
        auto dataId = atoi(iter->first.c_str());
        if (dataId / 1000000 == 205) ////背包中总英雄数量
        {
            heroVec->push_back(make_pair(dataId, iter->second.asInt()));
        }
    }
}

//更新物品数量 反回 -1 物品不够 -2 背包已满  num:正负数
int BattlePubUI::UpdateBackPackNum(int cfgId, int num)
{
    int backPackNum = 0;
    int StorehouseNum = 0;
    
    bool isEnough = true;
    
    //获取仓库当前物品数量
    auto val = UserData::getInstance()->Get_MVI_Value(UserDataEnum::Storehouse, cfgId);
    if (val != Value::Null)
    {
        StorehouseNum = val.asInt();
    }
    StorehouseNum -= num;
    
    if (StorehouseNum < 0)
    {
        backPackNum = -1;
        isEnough = false;
    }
    if (isEnough)
    {
        //获取背包中当前物品的数量
        val = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, cfgId);
        if (val != Value::Null)
        {
            backPackNum = val.asInt() ;
        }
        backPackNum += num;
        
        //获取背包中所有物品总数量
        auto backPackTotalNum = get<0>(GetBackPackNowNum_RanksNowNum());
        //获取背包最大容量
        auto backPackMaxNum = UserData::getInstance()->GetUserData(UserDataEnum::BackPackMaxNum)->asInt();
        
        if (backPackTotalNum + num > backPackMaxNum || backPackNum < 0)
        {
            backPackNum = -2;
            isEnough = false;
        }
    }
    
    if (isEnough)
    {
        // 每次点击并不进行io操作，在unload处saveData
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::BackPack, cfgId, num, false, true, false);
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, cfgId, -num, false, true, false);
    }
    return backPackNum;
}

//计算 当前背包数量 和 队伍数量
tuple<int, int> BattlePubUI::GetBackPackNowNum_RanksNowNum()
{
    auto dataId = 0;
    auto nowRanksNum = 0;
    auto nowBackPackNum = 0;
    
    auto vm_BackPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    
    for (auto iter = vm_BackPack->begin(); iter != vm_BackPack->end(); iter++)
    {
        dataId = atoi(iter->first.c_str());
        if (dataId / 1000000 == 205)
        {
            nowRanksNum += iter->second.asInt();
        }else
        {
            CfgDataRow item(dataId);
            if (item.GetValue(CfgField::NonSpace)->asInt() == 1) {
                nowBackPackNum += iter->second.asInt();
            }
        }
    }
    return make_tuple(nowBackPackNum, nowRanksNum);

}

//更新 背包容量 和 队伍人数 文本
void BattlePubUI::UpdateBackPackText_RanksText()
{
    auto ranksMaxNum = UserData::getInstance()->GetUserData(UserDataEnum::RanksMaxNum)->asInt();

    auto backPackMaxNum = UserData::getInstance()->GetUserData(UserDataEnum::BackPackMaxNum)->asInt();
    
    auto tuple_BackPackNowNum_RanksNowNum  = GetBackPackNowNum_RanksNowNum();
    int nowBackPackNum = get<0>(tuple_BackPackNowNum_RanksNowNum);
    int nowRankNum = get<1>(tuple_BackPackNowNum_RanksNowNum);
    
    auto layout_Left = m_RootWidget->getChildByName<Layout*>("Layout_Left");
    auto text_BagNum = layout_Left->getChildByName<Text*>("Text_BagNum");
    auto text = StringUtils::format("%d/%d", nowBackPackNum, backPackMaxNum);
    text_BagNum->setString(text);
    
    auto text_RanksNum = layout_Left->getChildByName<Text*>("Text_RanksNum");
    text = StringUtils::format("%d/%d", nowRankNum, ranksMaxNum);
    text_RanksNum->setString(text);
    
}


void BattlePubUI::ShowButton()
{
    
    Button* btn;
    ValueVector* vv;
    auto layout_Left = m_RootWidget->getChildByName<Layout*>("Layout_Left");
    
    vv = &UserData::getInstance()->GetUserData(UserDataEnum::FinishAchievement)->asValueVector();
    if (!vv->empty())
    {
        btn = layout_Left->getChildByName<Button*>("Btn_Achievement");
        btn->setVisible(true);
    }
    
    vv = &UserData::getInstance()->GetUserData(UserDataEnum::TalentList)->asValueVector();
    if (!vv->empty())
    {
        btn = layout_Left->getChildByName<Button*>("Btn_Talent");
        btn->setVisible(true);
    }
    
    if(UserData::getInstance()->GetUserData(UserDataEnum::TotalExploreValue)->asInt() > 0)
    {
        btn = layout_Left->getChildByName<Button*>("Btn_Ranking");
        btn->setVisible(true);
    }

}


void BattlePubUI::LoadFinish()
{
    ShowButton();
    
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    ProductionItem::getInstance()->SetSchedule(m_RootWidget);
    
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto btn_Right = layout_title->getChildByName<Button*>("Btn_Right");
    
    btn_Right->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
        auto scene = MainScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
    });
    
    
    auto layout_Left = m_RootWidget->getChildByName<Layout*>("Layout_Left");
    
    auto btn_Ranking = layout_Left->getChildByName<Button*>("Btn_Ranking");
    auto langId = (int)LanguageIdEnum::BtnRankingListLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    
    btn_Ranking->setTitleText(langText);
    btn_Ranking->addClickEventListener([this](Ref* sender)
         {
             this->UnLoad();
             auto scene = RankScene::CreateScene();
             Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
         });

    
    auto btn_Achievement = layout_Left->getChildByName<Button*>("Btn_Achievement");
    
    langId = (int)LanguageIdEnum::BtnMedalLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    
    btn_Achievement->setTitleText(langText);
    btn_Achievement->addClickEventListener([this](Ref* sender)
       {
           this->UnLoad();
           auto scene = AchievementScene::createScene();
           Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
       });
    
    auto btn_Talent = layout_Left->getChildByName<Button*>("Btn_Talent");
    langId = (int)LanguageIdEnum::BtnScienceLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    
    btn_Talent->setTitleText(langText);
    btn_Talent->addClickEventListener([this](Ref* sender)
       {
           this->UnLoad();
           auto scene = TalentScene::createScene();
           Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
       });

    auto btn_Battle = layout_Left->getChildByName<Button*>("Btn_Battle");
    langId = (int)LanguageIdEnum::BtnGoBattleLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    
    btn_Battle->setTitleText(langText);
    btn_Battle->addClickEventListener([this](Ref* sender)
      {
          
          int breadNum = 0;
          auto breadData = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, (int)CfgIdEnum::BreadId);
          if (breadData != Value::Null)
          {
              breadNum = breadData.asInt();
          }
          
          if (breadNum < 1)
          {
              auto langId = (int)LanguageIdEnum::MustCarryFoodLangId;
              auto langText = MultiLanguage::getInstance()->GetText(langId);
              MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
          }else if( GetBackPackHeroNum() <= 0)
          {
              auto langId = (int)LanguageIdEnum::MustCarryHeroLangId;
              auto langText = MultiLanguage::getInstance()->GetText(langId);
              MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
          }else
          {
              this->UnLoad();
              auto scene = MapScene::createScene();
              Director::getInstance()->replaceScene(CCTransitionFade::create(0.8f, scene, Color3B::BLACK));
          }
      });
    
    UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                          nullptr,
                                          [this]()
                                          {
                                              this->UnLoad();
                                              auto scene = MainScene::createScene();
                                              Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
                                          });
    
}


