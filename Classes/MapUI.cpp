//
//  MapUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MapUI.h"
#include "DeathUI.h"
#include "WineBarUI.h"
#include "MarketUI.h"
#include "MapScene.h"
#include "BattleUI.h"
#include "MainScene.h"
#include "TiledMapUI.h"
#include "BattleData.h"
#include "HoldBuildUI.h"
#include "BattlePubUI.h"
#include "MistTiledMapUI.h"
#include "AchievementSystem.h"
#include "BattleBagScene.h"

#define SHOW_REDBLOOD_BREADNUM          5

MapUI* MapUI::sm_MapUI = nullptr;

MapUI* MapUI::getInstance()
{
    if (sm_MapUI == nullptr)
    {
        sm_MapUI = new (std::nothrow) MapUI();
        if (sm_MapUI->Init()) {}
    }
    return sm_MapUI;
}

bool MapUI::Init()
{
    UserEventEntity eventEntity;
    eventEntity.m_UserEventType = UserEventType::BattleFinish;
    eventEntity.m_CallBack = [=](const int buildId, const Value& value)
    {
        CCLOGINFO("BattleFinish buildId =%d", buildId);
        
        ShowBreadNum();
        
    };
    UserEventContainer::getInstance()->Register(eventEntity);
    
    eventEntity.m_UserEventType = UserEventType::CaptureBuildEvent;
    eventEntity.m_CallBack = [=](const int buildId, const Value& value)
    {
        CCLOGINFO("OccupyBuild buildId =%d", buildId);
        ShowBreadNum();
        CfgDataRow dataRow(buildId);
        
        UpdateMapExplore(dataRow.GetValue(CfgField::Discoverd)->asInt());
        
        //UserEventContainer::getInstance()->Send(UserEventType::CaptureBuildEvent, buildId);
        
        AchievementSystem::getInstance()->CheckAchievement(UserEventType::FinishMap, buildId);
        
        TiledMapUI::getInstance()->UpdateBuildStatus(value.asInt(), TiledMapUI::BuildStatus::Occupy);
        
        ShowBattleSucUI(buildId, value.asBool());
    };
    UserEventContainer::getInstance()->Register(eventEntity);
    
    m_StoreBuyNumMap = new (std::nothrow) unordered_map<int, unordered_map<int, int>>();
    return true;
}

void MapUI::ReleaseInstance()
{
    m_StoreBuyNumMap->clear();
    delete m_StoreBuyNumMap;
    m_StoreBuyNumMap = nullptr;
    
    delete sm_MapUI;
    sm_MapUI = nullptr;
}

void MapUI::UnLoad()
{
    TiledMapUI::getInstance()->UnLoad();
    ProductionItem::getInstance()->UnSchedule();
    UIBase::UnLoad();
}

void MapUI::UpdateUI()
{
    ShowBreadNum();
    UpdateBackPack();
    UpdateMapExplore();
    ResetMarketBuyNum();
    TiledMapUI::getInstance()->Load(m_RootLayer);
}

Layout* MapUI::GetLayoutDirection()
{
    return m_RootWidget->getChildByName<Layout*>("Layout_Direction");
}

//在地图上碰到了障碍 buildIdOrMonsterId true是建筑Id false 敌方Id
void MapUI::MapObstacle(uint32_t tiledIdx ,uint32_t cfgId, bool buildIdOrMonsterId)
{
    int last4Num = cfgId / 1000 % 10;
    if (cfgId == (int)CfgIdEnum::EternalArena)
    {
        // 虚空竞技场特殊处理，需解锁，但倒数第4个尾数并不为2、3
        last4Num = 2;
    }
    
    if (buildIdOrMonsterId)
    {
        switch (cfgId)
        {
            case (int)CfgIdEnum::MapCastleBuildId :
                GoMainUI(true);
                break;
            case (int)CfgIdEnum::MapDeadBuildId :
                DeathBuild(tiledIdx ,cfgId);
                break;
            case (int)CfgIdEnum::MapPortalBuildId :
                PortalBuild(tiledIdx ,cfgId);
                break;
            default:
                switch (last4Num)
                {
                    case (int)CfgIdEnum::BuildMarketIdLast4 :
                        MarketBuild(tiledIdx ,cfgId);
                        break;
                    case (int)CfgIdEnum::BuildWineBarIdLast4 :
                        WineBarBuild(tiledIdx ,cfgId);
                        break;
//                    并没有相应的建筑id
//                    case (int)CfgIdEnum::BuildAthleticsIdLast4 :
//                        MonsterBuild(tiledIdx, cfgId); //竞技场
//                        break;
                    case (int)CfgIdEnum::BuildLock2Last4:
                        GoUnLockUI(tiledIdx, cfgId);
                        break;
                    case (int)CfgIdEnum::BuildLock1Last4:
                        GoUnLockUI(tiledIdx, cfgId);
                        break;
                    default:
                        OtherBuild(tiledIdx ,cfgId);
                        break;
                } ;
                break;
        }
    }else
    {
        Text * text = m_RootWidget->getChildByName<Text *>("ContryText");
        // 野外遇怪特效
        if (!text) {
            text = Text::create();
            m_RootWidget->addChild(text, 999);
            text->setPosition(Vec2(m_RootWidget->getContentSize().width/2, m_RootWidget->getContentSize().height/2));
            text->setString("敌袭");
            text->setFontSize(24);
            text->setFontName("FZZYJW.TTF");
            text->setName("ContryText");
        }
        text->setVisible(true);
        text->runAction(
                        CCSequence::create(CCScaleTo::create(0.5, 1.25),
                                           CCScaleTo::create(0.5, 0.8),
                                           CallFunc::create(
                                                            [=]()
                                                            {
                                                                BattleData::getInstance()->m_TiledIdx = tiledIdx;
                                                                BattleData::getInstance()->m_MonsterId = cfgId;
                                                                BattleData::getInstance()->m_BuildId = 0;
                                                                BattleData::getInstance()->m_NextBuildLayer = 0;
                                                                BattleUI::getInstance()->Load(m_RootLayer, "BattleUI.csb");
                                                                BattleUI::getInstance()->Show();
                                                                text->setVisible(false);
                                                            }),
                                           NULL));

        
        
//        BattleData::getInstance()->m_TiledIdx = tiledIdx;
//        BattleData::getInstance()->m_MonsterId = cfgId;
//        BattleData::getInstance()->m_BuildId = 0;
//        BattleData::getInstance()->m_NextBuildLayer = 0;
//        BattleUI::getInstance()->Load(m_RootLayer, "BattleUI.csb");
//        BattleUI::getInstance()->Show();
    }
}

void MapUI::GoMainUI(bool isDirect)
{
    bool isBack = true;
    if(!isDirect) {
        isBack = UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::BackPackBackHome, -1, false, false, true) >= 0;
        if(isBack) {
            RecordUtils::getInstance()->sdkRecord((int)umengEventID::Press_Back_Home, "消耗物品");
        }
    }
    
    if (!isBack && !isDirect)
    {
        auto showText = MultiLanguage::getInstance()->GetText((uint32_t)LanguageIdEnum::BackHomeId);
        UIUtils::getInstance()->addLeadBuyLayer(this, showText, 15,
                                                [=]()
                                                {
                                                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::BackPackBackHome, 1, false, false, true);
                                                    UpdateBackPack();
                                                    GoMainUI(true);
                                                });
        return;
    }
    UnLoad();
    BattleUI::getInstance()->TownPortal();
    BattleUI::getInstance()->UnLoad();
    auto scene = MainScene::createScene();
    Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
}

//占领后的建筑
void MapUI::HoldBuild(uint32_t tiledIdx ,uint32_t cfgId)
{
    uint32_t buildId = TiledMapUI::getInstance()->GetBuildId(tiledIdx);
    CfgDataRow dataRow(buildId);
    auto vm_item = &dataRow.GetValue(CfgField::OutputItemID_M)->asIntMultiKeyMap();
    if (vm_item->empty())
    {
        TiledMapUI::getInstance()->SetTouchEnabled(true);
    }
    else
    {
        HoldBuildUI::getInstance()->Load(m_RootLayer, "HoldBuildUI.csb", Value((int)tiledIdx));
        HoldBuildUI::getInstance()->Show();
    }
    
}

//酒吧建筑
void MapUI::WineBarBuild(uint32_t tiledIdx ,uint32_t cfgId)
{
    UIUtils::getInstance()->mapBuildingDialog(m_RootWidget, tiledIdx, cfgId,
                                              [=]()
                                              {
                                                  WineBarUI::getInstance()->Load(m_RootLayer, "WineBarUI.csb", Value((int)cfgId));
                                                  WineBarUI::getInstance()->Show();
                                              },
                                              []()
                                              {
                                                  TiledMapUI::getInstance()->SetTouchEnabled(true);
                                              },
                                              true
                                              );
}

//黑市建筑
void MapUI::MarketBuild(uint32_t tiledIdx ,uint32_t cfgId)
{
    UIUtils::getInstance()->mapBuildingDialog(m_RootWidget, tiledIdx, cfgId,
                                              [=]()
                                              {
                                                  MarketUI::getInstance()->Load(m_RootLayer, "MarketUI.csb", Value((int)cfgId));
                                                  MarketUI::getInstance()->Show();
                                              },
                                              []()
                                              {
                                                  TiledMapUI::getInstance()->SetTouchEnabled(true);
                                              },
                                              true
                                              );
}

//墓地建筑
void MapUI::DeathBuild(uint32_t tiledIdx ,uint32_t cfgId)
{
    UIUtils::getInstance()->mapBuildingDialog(m_RootWidget, tiledIdx, cfgId,
                                              [=]()
                                              {
                                                  DeathUI::getInstance()->Load(m_RootLayer, "DeathUI.csb");
                                                  DeathUI::getInstance()->Show();
                                              },
                                              []()
                                              {
                                                  TiledMapUI::getInstance()->SetTouchEnabled(true);
                                              },
                                              true
                                              );

}

//弹出进入新地图对话框
void MapUI::PortalBuild(uint32_t tiledIdx ,uint32_t cfgId)
{
    auto layout_PortalDialog = m_RootWidget->getChildByName<Layout*>("Layout_PortalDialog");
    layout_PortalDialog->setVisible(true);
    
    CfgDataRow dataRow(cfgId);
    
    auto text_PortalName = layout_PortalDialog->getChildByName<Text*>("Text_PortalName");
    auto langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_PortalName->setString(langText);
    
    auto text_PortalText = layout_PortalDialog->getChildByName<Text*>("Text_PortalText");
    auto btn_EnterPortal = layout_PortalDialog->getChildByName<Button*>("Btn_EnterPortal");
    
    langId = (int)LanguageIdEnum::EnterLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_EnterPortal->setTitleText(langText);
    
    auto mapNum = UserData::getInstance()->GetUserData(UserDataEnum::ExploreMaps)->asValueMap().size();
    if (mapNum > 1)
    {
        btn_EnterPortal->setVisible(true);
        btn_EnterPortal->addClickEventListener([this, layout_PortalDialog, tiledIdx, cfgId](Ref* sender)
           {
               ShowOptionMap(tiledIdx, cfgId);
               layout_PortalDialog->setVisible(false);
           });
        langId = dataRow.GetValue(CfgField::BodescribeId)->asInt();
    }else
    {
        langId = dataRow.GetValue(CfgField::UnlockMapID)->asInt();
        btn_EnterPortal->setVisible(false);
    }
    
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_PortalText->setString(langText);
    
    auto btn_ExitPortal = layout_PortalDialog->getChildByName<Button*>("Btn_ExitPortal");
    
    langId = (int)LanguageIdEnum::ExitLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_ExitPortal->setTitleText(langText);
    
    btn_ExitPortal->addClickEventListener([layout_PortalDialog](Ref* sender)
      {
          layout_PortalDialog->setVisible(false);
          TiledMapUI::getInstance()->SetTouchEnabled(true);
      });
    
}

//选择地图界面
void MapUI::ShowOptionMap(uint32_t tiledIdx, uint32_t cfgId)
{
    auto layout_PortalOption = m_RootWidget->getChildByName<Layout*>("Layout_PortalOption");
    layout_PortalOption->setVisible(true);
    auto text_TitleOption = layout_PortalOption->getChildByName<Text*>("Text_TitleOption");
    CfgDataRow dataRow(cfgId);
    auto langId = dataRow.GetValue(CfgField::ChoseMapID)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_TitleOption->setString(langText);
    
    auto btn_Cancel = layout_PortalOption->getChildByName<Button*>("Btn_Cancel");
    langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::CancelLangId);
    btn_Cancel->setTitleText(langText);
    btn_Cancel->addClickEventListener([layout_PortalOption](Ref* sender)
    {
        layout_PortalOption->setVisible(false);
        TiledMapUI::getInstance()->SetTouchEnabled(true);
    });
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    auto layout_Button = layout_Template->getChildByName<Layout*>("Layout_Button");
    
    auto lview_MapList = layout_PortalOption->getChildByName<ListView*>("LView_MapList");
    lview_MapList->removeAllChildren();
    
    auto exploreMaps = &UserData::getInstance()->GetUserData(UserDataEnum::ExploreMaps)->asValueMap();
    
    vector<pair<int, Value>> viv_ExploreMaps;
    UserData::getInstance()->VMSKToVIV_Sort(exploreMaps, &viv_ExploreMaps);
    
    int idx = 0;
    auto iter = viv_ExploreMaps.begin();
    while (iter != viv_ExploreMaps.end())
    {
        auto mapId = iter->first;
//        idx++;
        iter++;
        
        auto layout_tem = layout_Button->clone();
        
        CfgDataRow dataRow(mapId);
        
        idx = mapId % 9090000;
        
        langId = dataRow.GetValue(CfgField::Name)->asInt();
        langText = MultiLanguage::getInstance()->GetText(langId);
        
        auto btn_MapName = layout_tem->getChildByName<Button*>("Btn_MapName");
        btn_MapName->setTitleText(StringUtils::format("%d.%s", idx, langText.c_str()));
        btn_MapName->addClickEventListener([this, mapId](Ref* sender)
           {
               this->UnLoad();
               
               UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapId, Value(mapId), false);
               UserData::getInstance()->Update_Value(UserDataEnum::PlayerInMapTiledId, Value(-1));
               
               UserEventContainer::getInstance()->Send(UserEventType::GoToNewMapEvent, mapId);
               
               auto scene = MapScene::createScene();
               Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
           });
        lview_MapList->addChild(layout_tem);
    }
    lview_MapList->refreshView();
}

void MapUI::OtherBuild(uint32_t tiledIdx ,uint32_t cfgId)
{
    auto buildStatus = TiledMapUI::getInstance()->GetBuildStatus(tiledIdx);
    switch (buildStatus)
    {
        case TiledMapUI::BuildStatus::Normal :
             MonsterBuild(tiledIdx, cfgId);
            
            break;
        case TiledMapUI::BuildStatus::Occupy :
            GoUnLockUI(tiledIdx, cfgId);
//            HoldBuild(tiledIdx, cfgId);
            break;
            
        default:
            TiledMapUI::getInstance()->SetTouchEnabled(true);
            break;
    }
        
}

//建筑解锁 反回是否缺少物品， buildId:建筑Id  lackItemData：缺少的物品数据
bool MapUI::IsUnLockBuild(int buildId, vector<pair<int, int> >* lackItemData)
{
    // 判断是否钥匙解锁建筑以及是否有万能解锁器的科技
    if( (buildId / 1000 % 10) == (int)CfgIdEnum::BuildLock2Last4 && UserData::getInstance()->Check_VI_Fun(UserDataEnum::TalentList, Value((int)CfgIdEnum::TalentDecompilerID)))
    {
        return true;
    }
    
    auto vm_UnLockBuild = &UserData::getInstance()->GetUserData(UserDataEnum::UnLockBuildMaps)->asValueMap();
    auto mapBuildList = vm_UnLockBuild->find(StringUtils::format("%d", m_MapId));
    
    ValueVector* builds;
    if (mapBuildList != vm_UnLockBuild->end())
    {
        builds = &mapBuildList->second.asValueVector();
        auto unLockBuild_Iter = find(builds->begin(), builds->end(), Value(buildId));
        if(unLockBuild_Iter == builds->end())
        {
            return true;
        }
    }
    
    CfgDataRow dataRow(buildId);
    auto unLockItem = &dataRow.GetValue(CfgField::KeyItemID_M)->asIntMultiKeyMap();
    
    vector<pair<int, int> > lackData; //钥匙必须放背包里
    
    CheckBackPackUnLoackItem(unLockItem, &lackData);
    if(lackData.empty())
    {
        for (auto iter = unLockItem->begin(); iter != unLockItem->end(); iter++)
        {
            UserData::getInstance()->Update_VMI_Fun(UserDataEnum::BackPack, iter->first, iter->second.asInt() * -1);
        }
        auto unLockBuild_Iter = find(builds->begin(), builds->end(), Value(buildId));
        if(unLockBuild_Iter != builds->end())
        {
            builds->erase(unLockBuild_Iter);
//            UserData::getInstance()->SaveUserData();
            UserData::getInstance()->SaveCorUserData(false);
        }
        return true;
    }
    if (dataRow.GetValue(CfgField::KeyItem)->asInt() == 1)//钥匙
    {
        for (auto data : lackData)
        {
            lackItemData->push_back(data);
        }
    }else //仓库的解锁物品
    {
        ValueMapIntMultiKey vmimk;
        for (auto ulItem : lackData)
        {
            vmimk.insert(make_pair(ulItem.first, Value(ulItem.second)));
        }
        vector<pair<int, int> > storehouseLackData;
        bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vmimk , &storehouseLackData);
        if (delResutl)
        {
            
            for (auto iter = unLockItem->begin(); iter != unLockItem->end(); iter++)
            {
                auto num = iter->second.asInt();
                for(auto data : lackData)
                {
                    if (data.first == iter->first)
                    {
                        num -= data.second; break;
                    }
                }
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::BackPack, iter->first, iter->second.asInt() * -1);
            }
            
            auto unLockBuild_Iter = find(builds->begin(), builds->end(), Value(buildId));
            if(unLockBuild_Iter != builds->end())
            {
                builds->erase(unLockBuild_Iter);
//                UserData::getInstance()->SaveUserData();
                UserData::getInstance()->SaveCorUserData(false);
            }
            return true;
        }else
        {
            for (auto data : storehouseLackData)
            {
                lackItemData->push_back(data);
            }
        }
    }
    return false;
    
}

//获取背包里 缺少的解锁物品
void MapUI::CheckBackPackUnLoackItem(const ValueMapIntMultiKey* unLockItem, vector<pair<int, int> >* lackData)
{
    auto backPackData = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
    
    for (auto iter = unLockItem->begin(); iter != unLockItem->end(); iter++)
    {
        auto itemId = iter->first;
        auto strItemId = StringUtils::format("%d", itemId);
        auto si = backPackData->find(strItemId);
        if (si == backPackData->end())
        {
            lackData->push_back(make_pair(itemId, iter->second.asInt()));
        }else
        {
            if (si->second.asInt() < iter->second.asInt())
            {
                lackData->push_back(make_pair(itemId, iter->second.asInt() - si->second.asInt()));
            }
        }
    }
    
}

void MapUI::MonsterBuild(uint32_t tiledIdx ,uint32_t cfgId)
{
    auto layout_BuildDialog = m_RootWidget->getChildByName<Layout*>("Layout_BuildDialog");
    layout_BuildDialog->setVisible(true);
    
    CfgDataRow dataRow(cfgId);
    
    auto text_BuildName = layout_BuildDialog->getChildByName<Text*>("Text_BuildName");
    auto langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_BuildName->setString(langText);
    
    
    auto text_Desc = layout_BuildDialog->getChildByName<Text*>("Text_Desc");
    //占领前
    langId = dataRow.GetValue(CfgField::BodescribeId)->asInt();
    //占领后
    //langId = dataRow.GetValue(CfgField::AodescribeId)->asInt();
    //解锁描述
    //langId = dataRow.GetValue(CfgField::UnlockId)->asInt();
    
    langText = MultiLanguage::getInstance()->GetText(langId);
    
    text_Desc->setString(langText);
    
    auto btn_Enter = layout_BuildDialog->getChildByName<Button*>("Btn_Enter");
    langId = (int)LanguageIdEnum::EnterLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Enter->setTitleText(langText);
    btn_Enter->addClickEventListener([this, tiledIdx, cfgId, layout_BuildDialog](Ref* sender)
     {
         //初始化建筑层次
         ShowBattleDialog(tiledIdx, cfgId, 1);
         layout_BuildDialog->setVisible(false);
     });
    
    auto btn_Exit = layout_BuildDialog->getChildByName<Button*>("Btn_ExitBuild");
    langId = (int)LanguageIdEnum::ExitLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Exit->setTitleText(langText);
    
    btn_Exit->addClickEventListener([this, layout_BuildDialog](Ref* sender)
    {
        layout_BuildDialog->setVisible(false);
        TiledMapUI::getInstance()->SetTouchEnabled(true);
    });
}

//显示战斗对话框
void MapUI::ShowBattleDialog(uint32_t tiledIdx, uint32_t buildId, uint16_t nowBuildLayer)
{
    CfgDataRow dataRow(buildId);
    auto maxLayer = dataRow.GetValue(CfgField::Stories)->asInt();
    
    if (buildId > 0 && nowBuildLayer > maxLayer && buildId != (int)CfgIdEnum::EternalArena)
    {
        //占领了 这个建筑
        UserEventContainer::getInstance()->Send(UserEventType::CaptureBuildEvent, buildId, Value((int)tiledIdx));
        
        auto pass = dataRow.GetValue(CfgField::PassType)->asInt();
        
        if (pass != (int)BuildPass::NotPass)
        {
            if (pass == (int)BuildPass::PassNextMap)
            {
                auto nextMapId = m_MapId + 1;
                if(CfgData::getInstance()->GetCfgRow(nextMapId) != nullptr)
                {
                    if(UserData::getInstance()->Get_MVI_Value(UserDataEnum::ExploreMaps, nextMapId) == Value::Null)
                    {
                        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::ExploreMaps, nextMapId, 0);
                    }
                }
            }
            else if (pass == (int)BuildPass::PassNotNextMap)
            {
                // 通关图5、7、9要进入下一关需购买地图扩展包
                // checkIsBuyMap(m_MapId);
            }

        }
        
    }else
    {
        TiledMapUI::getInstance()->SetTouchEnabled(false);
        
        auto layout_BattleDialog = m_RootWidget->getChildByName<Layout*>("Layout_BattleDialog");
        
        layout_BattleDialog->setVisible(true);
        
        
        auto text_BattleName = layout_BattleDialog->getChildByName<Text*>("Text_BattleName");
        
        auto langId = dataRow.GetValue(CfgField::Bdstories)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        text_BattleName->setString(StringUtils::format(langText.c_str(), nowBuildLayer));

        auto text_Desc = layout_BattleDialog->getChildByName<Text*>("Text_Desc");
        langId = dataRow.GetValue(CfgField::UnlockId)->asInt();
        if(langId == 0) langId = dataRow.GetValue(CfgField::BodescribeId)->asInt();
        
        langText = MultiLanguage::getInstance()->GetText(langId);
        text_Desc->setString(langText);
        
        auto monstId = 0;
        
        if (buildId == (int)CfgIdEnum::EternalArena)
        {
            monstId = RandomEternalArenaMonsetr();
        }else
        {
            monstId = RandomMonsetr(buildId, nowBuildLayer); //获取建筑里的随机怪物
        }
        
        auto text_EnemyDesc = layout_BattleDialog->getChildByName<Text*>("Text_EnemyDesc");
        langId = CfgData::getInstance()->GetValue(monstId, CfgField::MonsterDescribe)->asInt();
        langText = MultiLanguage::getInstance()->GetText(langId);
        text_EnemyDesc->setString(langText);
        
        auto btn_Enter = layout_BattleDialog->getChildByName<Button*>("Btn_Battle");
        
        langId = (int)LanguageIdEnum::EnterLangId;
        langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Enter->setTitleText(langText);
        
        btn_Enter->addClickEventListener([=](Ref* sender)
         {
             BattleData::getInstance()->m_TiledIdx = tiledIdx;
             BattleData::getInstance()->m_MonsterId = monstId;
             BattleData::getInstance()->m_BuildId = buildId;
             BattleData::getInstance()->m_NextBuildLayer = nowBuildLayer + 1;
             
             BattleUI::getInstance()->Load(m_RootLayer, "BattleUI.csb");
             BattleUI::getInstance()->Show();
             layout_BattleDialog->setVisible(false);
         });
        
        auto btn_Exit = layout_BattleDialog->getChildByName<Button*>("Btn_ExitBattle");
        langId = (int)LanguageIdEnum::ExitLangId;
        langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Exit->setTitleText(langText);
        
        btn_Exit->addClickEventListener([this, layout_BattleDialog](Ref* sender)
        {
            layout_BattleDialog->setVisible(false);
            TiledMapUI::getInstance()->SetTouchEnabled(true);
        });
    }
}

bool MapUI::checkIsBuyMap(int curMapId)
{
    bool isBuy = false;
    int buyMapBagID = 0;
    if (curMapId == 909000005) {
        buyMapBagID = (int)CfgIdEnum::BuyMap6ID;
    }
    else if (curMapId == 909000007) {
        buyMapBagID = (int)CfgIdEnum::BuyMap8ID;
    }
    else if (curMapId == 909000009) {
        buyMapBagID = (int)CfgIdEnum::BuyMap10ID;
    }
    
    if (buyMapBagID == 0) {
        // 此关卡不需要购买拓展包，返回true可通下一关
        return true;
    }
    
    isBuy = DiamondBuyData::getInstance()->GetDiamondData(buyMapBagID)->asInt() == 1;
    
    if (!isBuy) {
        // 弹出购买框
        UIUtils::getInstance()->addLeadBuyMapLayer(this, curMapId,
                                                   []()
                                                   {
                                                       
                                                   });
    }
    return isBuy;
}

//计算一个随机一个永恒竞技场怪物
int MapUI::RandomEternalArenaMonsetr()
{
    auto weight = 0;
    auto totalWeight = 0;
    
    auto tableIdList = CfgData::getInstance()->GetCfgTableId("VoidMonster");
    
    vector<pair<int, int>> vec_pair;
    for (auto cfgId : *tableIdList)
    {
        CfgDataRow dataRow(cfgId);
        weight = dataRow.GetValue(CfgField::Fmeetpro)->asInt();
        totalWeight += weight;
        vec_pair.push_back(pair<int,int>(cfgId, weight));
    }
    auto randomSeed = RandomEngine::getInstance()->GetDefaultRandomSeed();
    auto monsterId = RandomEngine::getInstance()->GetRandomWeights(vec_pair, totalWeight, randomSeed);
    
    return monsterId;
}


//计算一个随机怪物 cfgId怪物出现表配置Id layerId层数
int MapUI::RandomMonsetr(int build, int layerId)
{
    auto monsterId = 0;
    CfgDataRow buildDataRow(build);
    //获取建筑里的怪物数据配置数据
    auto buildMonsterId = buildDataRow.GetValue(CfgField::MonsterconID)->asInt();
    
    CfgDataRow dataRow(buildMonsterId);
    auto vm_LayerMonster = dataRow.GetValue(CfgField::FMonsterID_M)->asIntMultiKeyMap();
    for (auto keyVal :  vm_LayerMonster)
    {
        if (keyVal.second.asInt() == layerId)
        {
            monsterId = keyVal.first; break;
        }
    }
    if (monsterId == 0)
    {
        auto totalWeight = 0;
        vector<pair<int, int>> vec_pair;
        auto vm_monster = dataRow.GetValue(CfgField::RMonsterID_M)->asIntMultiKeyMap();
        for (auto keyVal : vm_monster)
        {
            totalWeight +=  keyVal.second.asInt();
            vec_pair.push_back(pair<int,int>(keyVal.first, keyVal.second.asInt()));
        }
        auto randomSeed = RandomEngine::getInstance()->GetDefaultRandomSeed();
        monsterId = RandomEngine::getInstance()->GetRandomWeights(vec_pair, totalWeight, randomSeed);
    }
    return monsterId;
}


//更新地图探索数据 exploreVal:增加探索值
void MapUI::UpdateMapExplore(int addExploreVal)
{
    if (m_RootWidget == nullptr) return;
    
    auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    auto layout_LeftTop = layout_Title->getChildByName<Layout*>("Layout_LeftTop");
   
    
    //修改所有地图总探索值
    auto allMapExploreVal = UserData::getInstance()->GetUserData(UserDataEnum::TotalExploreValue)->asInt();
    if (addExploreVal > 0)
    {
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::ExploreMaps, m_MapId, addExploreVal);
        
        AchievementSystem::getInstance()->CheckAchievement(UserEventType::ExploreMap, addExploreVal);
    
        UserData::getInstance()->Update_Value(UserDataEnum::TotalExploreValue, Value(allMapExploreVal += addExploreVal));
    }
    
    auto text_ExploreVal = layout_LeftTop->getChildByName<Text*>("Text_ExploreVal");
    auto langId = (int)LanguageIdEnum::ExploreLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_ExploreVal->setString(langText);
    
    auto text_ExploreValNum = layout_LeftTop->getChildByName<Text*>("Text_ExploreValNum");
    text_ExploreValNum->setString(StringUtils::format("%d",allMapExploreVal));
    
    auto nowExploreVal = 0;
    auto mapData = UserData::getInstance()->Get_MVI_Value(UserDataEnum::ExploreMaps, m_MapId);
    //当前地图探索值
    if (mapData != Value::Null)
    {
        nowExploreVal = mapData.asInt();
    }
    
    if(addExploreVal > 0) nowExploreVal += addExploreVal;
    
    //计算当前地图总探索值
    if( addExploreVal == -1)
    {
        m_MapTotalExplore = 0;
        CfgDataRow dataRow(m_MapId);
        auto mapName = dataRow.GetValue(CfgField::MapBuilding_S)->asString();
        auto tableIdList = CfgData::getInstance()->GetCfgTableId(mapName);
        
        for (auto cfgId : *tableIdList)
        {
            CfgDataRow buildDataRow(cfgId);
            m_MapTotalExplore += buildDataRow.GetValue(CfgField::Discoverd)->asInt();
        }
    }
    
    auto explorePercent = (float)nowExploreVal / (float)m_MapTotalExplore  * 100;
    
    explorePercent = explorePercent > 100 ? 100 : explorePercent;
    
    auto text_ExplorePercent = layout_LeftTop->getChildByName<Text*>("Text_ExplorePercent");
    langId = (int)LanguageIdEnum::MapExploreLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_ExplorePercent->setString(langText);
    
    auto text_ExplorePercentNum = layout_LeftTop->getChildByName<Text*>("Text_ExplorePercentNum");
    text_ExplorePercentNum->setString(StringUtils::format("%d", (int)explorePercent) + "%");
    
}

//显示面包数量
void MapUI::ShowBreadNum()
{
    auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    auto layout_LeftTop = layout_Title->getChildByName<Layout*>("Layout_LeftTop");
    
    auto text_Bread = layout_LeftTop->getChildByName<Text*>("Text_Bread");
    auto text_BreadNum = layout_LeftTop->getChildByName<Text*>("Text_BreadNum");
    
    auto backPackBreadNum = 0;
    auto backPackBreadData = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, (int)CfgIdEnum::BreadId);
    if (backPackBreadData != Value::Null) backPackBreadNum = backPackBreadData.asInt();

    auto langId = (int)LanguageIdEnum::MapBreadLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_Bread->setString(langText);
    text_BreadNum->setString(StringUtils::format("%d", backPackBreadNum));
    
    ShowRedBlood(backPackBreadNum < SHOW_REDBLOOD_BREADNUM);
}

void MapUI::ShowBattleSucUI(int buildId, bool isSuccess)
{
    if (!isSuccess) {
        return;
    }
    
    CfgDataRow dataRow(buildId);
    auto langId = dataRow.GetValue(CfgField::VictoryId)->asInt();
    if(langId == 0)
    {
        // 无占领后描述，则说明不需要显示
        return;
    }
    
    auto langDescText = MultiLanguage::getInstance()->GetText(langId);
    langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto langNameText = MultiLanguage::getInstance()->GetText(langId);
    
    bool isOpenNewMap = (buildId / 1000 % 10 == (int)CfgIdEnum::BuildOpenNewMap);
    if (!isOpenNewMap)
    {
        // 显示占领界面
        auto layout_content = m_RootWidget->getChildByName<Layout *>("Layout_Context");
        layout_content->setVisible(true);
        
        auto text_title = layout_content->getChildByName<Text *>("Text_Title");
        text_title->setString(langNameText);
        
        auto text_desc = layout_content->getChildByName<Text *>("Text_SucDesc");
        text_desc->setString(langDescText);
        
        auto btn_close = layout_content->getChildByName<Button *>("Button_Close");
        btn_close->addClickEventListener(
                                         [=](Ref* sender)
                                         {
                                             layout_content->setVisible(false);
                                         });
        
        auto text_lackItem = layout_content->getChildByName<Text *>("Text_LackKey");
        auto btn_enter = layout_content->getChildByName<Button *>("Button_Enter");
        auto btn_close2 = layout_content->getChildByName<Button *>("Button_Close2");
        
        btn_close->setVisible(true);
        text_lackItem->setVisible(false);
        btn_enter->setVisible(false);
        btn_close2->setVisible(false);
        
    }
    else
    {
        // 播放剧情
        langId = dataRow.GetValue(CfgField::VictoryId)->asInt();
        auto msg = MultiLanguage::getInstance()->GetText(langId);
        
        vector<string> splitMsg;
        unsigned long startPlace = 0;
        unsigned long endPlace = msg.find('\n');
        string curMsg;
        string nextMsg = msg;
        while (endPlace != -1)
        {
            curMsg = nextMsg.substr(startPlace, endPlace);
            nextMsg = nextMsg.substr(endPlace+sizeof('\n'));
            endPlace = nextMsg.find('\n');
            splitMsg.push_back(curMsg);
        }
        
        auto layout_plot = m_RootWidget->getChildByName<Layout *>("Layout_Plot");
        if (!layout_plot) return;
        
        auto listview_plot = layout_plot->getChildByName<ListView *>("ListView_Plot");
        if (!listview_plot) return;
        
        auto btn_close = layout_plot->getChildByName<Button *>("Button_Close");
        if (!btn_close) return;
        btn_close->setVisible(false);
        
        auto firstItem = listview_plot->getItem(0);
        auto originPos = firstItem->getWorldPosition();
        
        layout_plot->setVisible(true);
        
        m_index = 0;
        layout_plot->schedule(
                                [=](float dt)
                                {
                                    if (m_index >= splitMsg.size()) {
                                        layout_plot->unschedule("PlayPlotInMap");
                                        btn_close->setVisible(true);
                                        // 通关图5、7、9要进入下一关需购买地图扩展包
                                        checkIsBuyMap(m_MapId);
                                    }
                                    else
                                    {
                                        addPlotMsg(splitMsg[m_index++], originPos);
                                    }
                                }, 3, "PlayPlotInMap");
        
        btn_close->addClickEventListener(
                                         [=](Ref* sender)
                                         {
                                             layout_plot->setVisible(false);
                                             listview_plot->removeAllChildren();
                                             listview_plot->addChild(Text::create());
                                         });
        
    }
    
}

void MapUI::ShowLackKeyUI(int tiled, int buildId, vector<pair<int, int> >* lackData, bool isLocked, bool isOccupy)
{
    if (buildId <= 0) {
        return;
    }
    
    CfgDataRow dataRow(buildId);
    
    auto langId = dataRow.GetValue(CfgField::BodescribeId)->asInt();
    
    auto langDescText = MultiLanguage::getInstance()->GetText(langId);
    langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto langNameText = MultiLanguage::getInstance()->GetText(langId);
    
    auto layout_content = m_RootWidget->getChildByName<Layout *>("Layout_Context");
    layout_content->setVisible(true);
    
    auto text_title = layout_content->getChildByName<Text *>("Text_Title");
    text_title->setString(langNameText);
    
    auto text_desc = layout_content->getChildByName<Text *>("Text_SucDesc");
    text_desc->setString(langDescText);
    
    auto text_lackItem = layout_content->getChildByName<Text *>("Text_LackKey");
    auto btn_enter = layout_content->getChildByName<Button *>("Button_Enter");
    auto btn_close2 = layout_content->getChildByName<Button *>("Button_Close2");
    auto btn_close = layout_content->getChildByName<Button *>("Button_Close");
    
    text_lackItem->setVisible(true);
    btn_enter->setVisible(true);
    btn_close2->setVisible(true);
    btn_close->setVisible(true);
    
    btn_close2->addClickEventListener(
                                      [=](Ref* sender)
                                      {
                                          layout_content->setVisible(false);
                                          TiledMapUI::getInstance()->SetTouchEnabled(true);
                                      });
    // true：已解锁，再判断是否已占领
    if (isLocked) {
        if (isOccupy) {
            langId = dataRow.GetValue(CfgField::AodescribeId)->asInt();
        }
        else
        {
            // BodescribeId UnlockId
            langId = dataRow.GetValue(CfgField::BodescribeId)->asInt();
        }
        if(langId != 0) langDescText = MultiLanguage::getInstance()->GetText(langId);
        
        text_desc->setString(langDescText);
        
        text_lackItem->setVisible(false);
        btn_close->setVisible(false);
        
        // 若为0，则可重复打，并无领取界面
        int octype = dataRow.GetValue(CfgField::Octype)->asInt();
        btn_enter->addClickEventListener(
                                         [=](Ref* sender)
                                         {
                                             if (isOccupy && octype != 0) {
                                                 // 进入领取
                                                 HoldBuild(tiled, buildId);
                                             }
                                             else
                                             {
                                                 // 进入
                                                 // MonsterBuild(tiled, buildId);
                                                 ShowBattleDialog(tiled, buildId, 1);
                                             }
                                             
                                             layout_content->setVisible(false);
                                         });
        return;
    }
    
    auto unLockItem = &dataRow.GetValue(CfgField::KeyItemID_M)->asIntMultiKeyMap();
    vector<pair<int, int> > lackData2; //钥匙必须放背包里
    // 判断是否缺少物品
    CheckBackPackUnLoackItem(unLockItem, &lackData2);
    if(!lackData2.empty())
    {
        btn_enter->setVisible(false);
        btn_close2->setVisible(false);
        btn_close->setVisible(true);
        
        int lackItemId = std::get<0>(lackData->front());
//        int lackItemNum = std::get<1>(lackData->front());
        CfgDataRow keyDataRow(lackItemId);
        langId = keyDataRow.GetValue(CfgField::Name)->asInt();
        auto langKeyName = MultiLanguage::getInstance()->GetText(langId);
        
        // lackData2不为空，未解锁，且背包无足够解锁物品
        text_lackItem->setVisible(true);
        text_lackItem->setString("缺少" + langKeyName);
        
        btn_close->addClickEventListener(
                                         [=](Ref* sender)
                                         {
                                             layout_content->setVisible(false);
                                             TiledMapUI::getInstance()->SetTouchEnabled(true);
                                         });
    }
    else
    {
        // lackData2为空，未解锁，且背包有足够的解锁物品
        text_lackItem->setVisible(false);
        btn_close->setVisible(false);
        
        btn_enter->addClickEventListener(
                                         [=](Ref* sender)
                                         {
                                             // 消耗物品
                                             vector<pair<int, int> > lackItemData;
                                             // 进入
                                             MonsterBuild(tiled, buildId);
                                             
                                             layout_content->setVisible(false);
                                         });
    }
    
}

void MapUI::addPlotMsg(string msg, Vec2 originPos)
{
    if (msg.length() <= 0)
    {
        return;
    }
    
    if (!m_RootWidget) {
        return;
    }
    
    auto layout_plot = m_RootWidget->getChildByName<Layout *>("Layout_Plot");
    if (!layout_plot) return;
    
    auto listview_plot = layout_plot->getChildByName<ListView *>("ListView_Plot");
    if (!listview_plot) return;
    
    auto layout_template = layout_plot->getChildByName<Text *>("Text_Template");
    if (!layout_template) return;
    
    auto textItem = (Text *)layout_template->clone();
    textItem->ignoreContentAdaptWithSize(false);
    textItem->setString(msg);
    textItem->setTextHorizontalAlignment(TextHAlignment::LEFT);

    int rows = 1;
    std::u16string utf16String;
    if(StringUtils::UTF8ToUTF16(msg, utf16String))
    {
        rows += ceil(utf16String.length() / LIST_ITEM_HORWORD);
    }
    Size textAreaSize(listview_plot->getContentSize().width, rows * textItem->getContentSize().height);
    textItem->setTextAreaSize(textAreaSize);
    textItem->setPosition(Vec2(originPos.x, originPos.y + rows * LIST_ITEM_HEIGHT));

    //    m_RootWidget->addChild(textItem, 100);
//    textItem->runAction(CCSequence::create(
//                                 CCMoveTo::create(LIST_MOVE_DURATION, originPos),
//                                 CallFunc::create(
//                                                  [=]()
//                                                  {
//                                                      textItem->removeFromParent();
//                                                      listview_plot->insertCustomItem(textItem, 0);
//                                                  }),
//                                 nullptr));
    
    listview_plot->addChild(textItem);
    
//    for (auto item : listview_plot->getItems())
//    {
//        auto pos = item->getPosition();
//        item->runAction(CCMoveTo::create(LIST_MOVE_DURATION, Vec2(pos.x, pos.y - item->getContentSize().height)));
//    }
    
}

void MapUI::GoUnLockUI(int tiled, int buildId)
{
    auto buildStatus = TiledMapUI::getInstance()->GetBuildStatus(tiled);
    bool isOccupy = buildStatus == TiledMapUI::BuildStatus::Occupy;
    vector<pair<int, int> > lackItemData;
    bool isLock = IsUnLockBuild(buildId, &lackItemData);
    ShowLackKeyUI(tiled, buildId, &lackItemData, isLock, isOccupy);
}

void MapUI::ShowMoveDirection(MoveDirection md)
{
    auto layout_Direction = m_RootWidget->getChildByName<Layout*>("Layout_Direction");
    layout_Direction->setVisible(true);
    
    string imgName = "";
    switch (md)
    {
        case MoveDirection::Left:
        {
            imgName = "ImgLeft";    break;
        }
        case MoveDirection::Right:
        {
            imgName = "ImgRight";    break;
        }
        case MoveDirection::Up:
        {
            imgName = "ImgUp";    break;
        }
        case MoveDirection::Down:
        {
            imgName = "ImgDown";    break;
        }
    }
    auto img =  layout_Direction->getChildByName<ImageView*>(imgName);
    img->setVisible(true);

}

void MapUI::HideMoveDirection()
{
    auto layout_Direction = m_RootWidget->getChildByName<Layout*>("Layout_Direction");
    layout_Direction->setVisible(false);
    
    auto imgLeft =  layout_Direction->getChildByName<ImageView*>("ImgLeft");
    if (imgLeft->isVisible()) imgLeft->setVisible(false);
    
    auto imgRight =  layout_Direction->getChildByName<ImageView*>("ImgRight");
    if (imgRight->isVisible()) imgRight->setVisible(false);
    
    auto imgUp =  layout_Direction->getChildByName<ImageView*>("ImgUp");
    if (imgUp->isVisible()) imgUp->setVisible(false);
    
    auto imgDown =  layout_Direction->getChildByName<ImageView*>("ImgDown");
    if (imgDown->isVisible()) imgDown->setVisible(false);
    
}

//更新面包数量
//反回是否有足够的面包
bool MapUI::UpdateBreadNum()
{
    bool result = true;
    
    if(m_RootWidget == nullptr)
    {
        result = false;
    }else
    {
        m_BreadNowMoveTiledNum++;
        if (m_BreadNowMoveTiledNum == m_BreadMaxMoveTiledNum)
        {
            m_BreadNowMoveTiledNum = 0;
            
            auto backPackBreadNum = 0;
            auto backPackBreadData = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, (int)CfgIdEnum::BreadId);
            if (backPackBreadData != Value::Null) backPackBreadNum = backPackBreadData.asInt();
            if(backPackBreadNum >= 1)
            {
                // best test
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::BackPack, (int)CfgIdEnum::BreadId, -1);
            }
            if(backPackBreadNum == 0)
            {
                BattleUI::getInstance()->HeroDeath(true);
                BattleUI::getInstance()->UnLoad(); UnLoad();
                Director::getInstance()->replaceScene(CCTransitionFade::create(0.8f, MainScene::createScene(), Color3B::BLACK));
                result = false;
            }else
            {
                UpdateBackPack();
                ShowBreadNum();
            }
        }
    }
    return result;
}

//是否显示红色遮罩（面包小于5）
void MapUI::ShowRedBlood(bool isShow)
{
    auto layout_redBlood = m_RootWidget->getChildByName<Layout *>("Layout_Blood");
    
    auto backPackBreadNum = 0;
    auto backPackBreadData = UserData::getInstance()->Get_MVI_Value(UserDataEnum::BackPack, (int)CfgIdEnum::BreadId);
    if (backPackBreadData != Value::Null) backPackBreadNum = backPackBreadData.asInt();
    
    if (backPackBreadNum < 5)
    {
        layout_redBlood->setVisible(true);
        auto imgBlood = layout_redBlood->getChildByName<Node *>("Image_Blood");
        if(!m_RootWidget->isScheduled("MapRedBlood"))
        {
            m_RootWidget->schedule(
            [=](float dt)
            {
                if (isUp)
                {
                    opacityNum += 10;
                }
                else
                {
                    opacityNum -= 10;
                }
                
                if (opacityNum >= 255) {
                    isUp = false;
                }
                else if (opacityNum <= 150)
                {
                    isUp = true;
                }
                imgBlood->setOpacity(opacityNum);
            }, 0.1, "MapRedBlood");
        }
    }
    else
    {
        layout_redBlood->setVisible(false);
        m_RootWidget->unschedule("MapRedBlood");
    }
    
}

void MapUI::ResetMarketBuyNum()
{
    m_StoreBuyNumMap->clear();
    CfgDataRow dataRow(m_MapId);
    
    int storeId;
    int itemId;
    int itemBuyNum;
    auto vv_storeId = &dataRow.GetValue(CfgField::FieldStoreID_V)->asValueVector();
    for(auto store : *vv_storeId) {
        storeId = store.asInt();
        CfgDataRow storeRow(storeId);
        
        auto storeIter = m_StoreBuyNumMap->find(storeId);
        if (storeIter == m_StoreBuyNumMap->end()) {
            unordered_map<int, int> temp;
            m_StoreBuyNumMap->insert(make_pair(storeId, temp));
        }
        
        auto vm_outputItem = &storeRow.GetValue(CfgField::OutputItemID_M)->asIntMultiKeyMap();
        auto iter = vm_outputItem->begin();
        while (iter != vm_outputItem->end()) {
            itemId = iter->first;
            itemBuyNum = iter->second.asInt();
            m_StoreBuyNumMap->find(storeId)->second.insert(make_pair(itemId, itemBuyNum));
            ++iter;
        }
    }
    
}

int MapUI::SetMarketBuyNum(int storeId, int fieldStoreID, int num)
{
    auto storeIter = m_StoreBuyNumMap->find(storeId);
    if (storeIter != m_StoreBuyNumMap->end()) {
        auto fieldIter = storeIter->second.find(fieldStoreID);
        if (fieldIter != storeIter->second.end()) {
            int curNum = fieldIter->second;
            if (num != 0) {
                curNum = curNum + num;
                fieldIter->second = curNum;
            }
            return curNum;
        }
    }
    
    return -1;
}

//更新的背包容量
void MapUI::UpdateBackPack()
{
    auto backPackMaxNum = UserData::getInstance()->GetUserData(UserDataEnum::BackPackMaxNum)->asInt();
    
    auto tuple_BackPackNowNum_RanksNowNum  =  BattlePubUI::getInstance()->GetBackPackNowNum_RanksNowNum();
    
    int nowBackPackNum = get<0>(tuple_BackPackNowNum_RanksNowNum);
    
    auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto text_BackPack = layout_Title->getChildByName<Text*>("Text_Backpack");
    text_BackPack->setString(StringUtils::format("%d/%d", nowBackPackNum, backPackMaxNum));
    
    auto backHomeIter = UserData::getInstance()->Get_MVI_Value(UserDataEnum::Storehouse, (int)CfgIdEnum::BackPackBackHome);
    auto backHomeNum = 0;
    if (backHomeIter != Value::Null) {
        backHomeNum = backHomeIter.asInt();
    }
    auto text_GoHome = layout_Title->getChildByName<Text*>("Text_GoHome");
    text_GoHome->setString(StringUtils::format("%d", backHomeNum));
}

//显示离开对话框
void MapUI::ShowExitDialog(uint32_t titleLangId, uint32_t descLangId)
{
    auto layout_ExitDialog = m_RootWidget->getChildByName<Layout*>("Layout_ExitDialog");
    
    layout_ExitDialog->setVisible(true);
    
    auto text_ExitTitle = layout_ExitDialog->getChildByName<Text*>("Text_ExitTitle");
    auto langText = MultiLanguage::getInstance()->GetText(titleLangId);
    text_ExitTitle->setString(langText);
    
    auto text_ExitDesc = layout_ExitDialog->getChildByName<Text*>("Text_ExitDesc");
    langText = MultiLanguage::getInstance()->GetText(descLangId);
    text_ExitDesc->setString(langText);
    
    auto btn_Exit = layout_ExitDialog->getChildByName<Button*>("Btn_ExitDialog");
    btn_Exit->addClickEventListener([this, layout_ExitDialog](Ref* sender)
     {
         layout_ExitDialog->setVisible(false);
         TiledMapUI::getInstance()->SetTouchEnabled(true);
     });
}


void MapUI::LoadFinish()
{
    // 根据已获得天赋获得 背包、地图探索等相关计算变量
    BattleData::getInstance()->initTalentNum(false);
    
    m_BreadNowMoveTiledNum = 0;
    
    ProductionItem::getInstance()->SetSchedule(m_RootWidget);
//    auto talentlist =  &UserData::getInstance()->GetUserData(UserDataEnum::TalentList)->asValueVector();
//    auto talent_iter = find(talentlist->begin(), talentlist->end(), Value((int)TalentCfgId::ReduceBreadId));
//    if (talent_iter != talentlist->end())
//    {
//        m_BreadMaxMoveTiledNum = 2; //2格式消耗一个面包
//    }else
//    {
//        m_BreadMaxMoveTiledNum = 1;
//    }
    m_BreadMaxMoveTiledNum = 1 + BattleData::getInstance()->m_TalentReduceBread;
    
    m_MapId = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapId)->asInt();
    
    auto layout_Title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    auto layout_LeftTop = layout_Title->getChildByName<Layout*>("Layout_LeftTop");
    
    auto text_MapName = layout_LeftTop->getChildByName<Text*>("Text_MapName");
    
    CfgDataRow dataRow(m_MapId);
    auto langId = dataRow.GetValue(CfgField::Name)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_MapName->setString(langText);

    
    auto btn_BackPack = layout_Title->getChildByName<Button*>("Btn_BackPack");
    langId = (int)LanguageIdEnum::BagNameLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_BackPack->setTitleText(langText);
    btn_BackPack->addClickEventListener([this](Ref* sender){
        UnLoad();
//        BattleUI::getInstance()->TownPortal();
        BattleUI::getInstance()->UnLoad();
        auto scene = BattleBagScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInR::create(0.18f, scene));
    });
    
    auto btn_Home = layout_Title->getChildByName<Button*>("Btn_GoHome");
    langId = (int)LanguageIdEnum::GoHomeLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Home->setTitleText(langText);
    btn_Home->addClickEventListener([this](Ref* sender){ GoMainUI(false); });
    
    
    auto mapId = UserData::getInstance()->GetUserData(UserDataEnum::PlayerInMapId)->asInt();
    auto strMapId = StringUtils::format("%d", mapId);
    auto vm_MapNotHoldBuild = &UserData::getInstance()->GetUserData(UserDataEnum::MapNotHoldBuild)->asValueMap();
    auto iter = vm_MapNotHoldBuild->find(strMapId);
    if (iter == vm_MapNotHoldBuild->end())
    {
        auto cfgId = 0;
        //int onlyType = 0;
        ValueVector vv;
        CfgDataRow dataRow(mapId);
        auto mapBuild = dataRow.GetValue(CfgField::MapBuilding_S)->asString();
        auto tableIdList = CfgData::getInstance()->GetCfgTableId(mapBuild);
        if (!tableIdList) return;
        for(auto iter = tableIdList->begin(); iter != tableIdList->end(); iter++)
        {
            cfgId = *iter;
            //onlyType = CfgData::getInstance()->GetValue(cfgId, CfgField::Octype)->asInt();
            //if (onlyType == 1)
            {
                vv.push_back(Value(cfgId));
            }
        }
        vm_MapNotHoldBuild->insert(make_pair(strMapId, Value(vv)));
    }
    
    
    auto vm_UnLockBuild = &UserData::getInstance()->GetUserData(UserDataEnum::UnLockBuildMaps)->asValueMap();
    iter = vm_UnLockBuild->find(strMapId);
    if (iter == vm_UnLockBuild->end())
    {
        auto cfgId = 0;
        ValueVector vv;
        CfgDataRow dataRow(mapId);
        auto mapBuild = dataRow.GetValue(CfgField::MapBuilding_S)->asString();
        auto tableIdList = CfgData::getInstance()->GetCfgTableId(mapBuild);
        for(auto iter = tableIdList->begin(); iter != tableIdList->end(); iter++)
        {
            cfgId = *iter;
            auto unlockitem = CfgData::getInstance()->GetValue(cfgId, CfgField::KeyItemID_M)->asIntMultiKeyMap();
            if (!unlockitem.empty())
            {
                vv.push_back(Value(cfgId));
            }
        }
        vm_UnLockBuild->insert(make_pair(strMapId, Value(vv)));
    }
    
    
    
}


