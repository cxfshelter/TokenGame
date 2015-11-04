//
//  MakeBuildUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MainScene.h"
#include "MakeBuildUI.h"

MakeBuildUI* MakeBuildUI::sm_MakeBuildUI = nullptr;


MakeBuildUI* MakeBuildUI::getInstance()
{
    if (sm_MakeBuildUI == nullptr)
    {
        sm_MakeBuildUI = new (std::nothrow) MakeBuildUI();
        if (sm_MakeBuildUI->Init()) {}
    }
    return sm_MakeBuildUI;
}

bool MakeBuildUI::Init()
{
    //建造兵工厂建筑
    auto buildId = (int)CfgIdEnum::ArsenalId;
    auto buildVal = UserData::getInstance()->Get_MVI_Value(UserDataEnum::MakeBuild, buildId);
    if (buildVal == Value::Null)
    {
        UserEventEntity eventEntity;
        eventEntity.m_UserEventType = UserEventType::CaptureBuildEvent;
        eventEntity.m_UserEventName = "AddArsenalId"; //增加兵工厂建筑Id
        eventEntity.m_CallBack = [buildId](const int id, const Value& value)
        {
            UserEventContainer::getInstance()->UnRegister(UserEventType::CaptureBuildEvent, "AddArsenalId");
            UserData::getInstance()->Update_VMI_Fun(UserDataEnum::MakeBuild, buildId, (int)MakeBuildStatus::NewBuild, true);
            
        };
        UserEventContainer::getInstance()->Register(eventEntity);
    }
    
    return true;
}

void MakeBuildUI::ReleaseInstance()
{
    delete sm_MakeBuildUI;
    sm_MakeBuildUI = nullptr;
}

void MakeBuildUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    ProductionItem::getInstance()->UnSchedule();
    ValueMap* vm_MakeBuild =  &(UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild)->asValueMap());
    
    //去掉新的建筑状态
    for (auto pair = vm_MakeBuild->begin(); pair != vm_MakeBuild->end(); pair++)
    {
        if (pair->second.asInt() == 0)
        {
            pair->second = Value(1); //1有建筑可建
        }
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    UIBase::UnLoad();
}

void MakeBuildUI::LoadFinish()
{
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    ProductionItem::getInstance()->SetSchedule(m_RootWidget);
    
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto btn_Left = layout_title->getChildByName<Button*>("Btn_Left");
    
    btn_Left->addClickEventListener([this](Ref* sender)
        {
            this->UnLoad();
            auto scene = MainScene::createScene();
            Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
        });
    
    UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                          [this]()
                                          {
                                              this->UnLoad();
                                              auto scene = MainScene::createScene();
                                              Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                          }, nullptr);
    
}


void MakeBuildUI::UpdateUI()
{
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    //有新建筑可建或有建筑可建
    auto layout_Item0 = layout_Template->getChildByName<Layout*>("Layout_Item0");
    //已建好
    auto layout_Item1 = layout_Template->getChildByName<Layout*>("Layout_Item1");

    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lView_List = layout_Center->getChildByName<ListView*>("LView_List");

    Layout* layout_Item;
    vector<pair<int, Value>> viv;
    //每次清空 子控件
    lView_List->removeAllChildren();
    ValueMap* vm_MakeBuild =  &(UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild)->asValueMap());
    vector<pair<int, Value>> viv_MakeBuild;
    UserData::getInstance()->VMSKToVIV_Sort(vm_MakeBuild, &viv_MakeBuild);
    
    for (auto pair = viv_MakeBuild.begin(); pair != viv_MakeBuild.end(); pair++)
    {
        auto cfgId = pair->first;
        CfgDataRow dataRow(cfgId);
        
        auto langId =  dataRow.GetValue(CfgField::Name)->asInt();
        auto buildName = MultiLanguage::getInstance()->GetText(langId);

        switch (pair->second.asInt()) {
            case 0:
            case 1:
                layout_Item = (Layout*)layout_Item0->clone();
                break;
            default:
                layout_Item = (Layout*)layout_Item1->clone();
                break;
        }
        layout_Item->setVisible(true);
        lView_List->addChild(layout_Item);

        auto btn_name = layout_Item->getChildByName<Button*>("Btn_Name");
        btn_name->setTitleText(buildName.c_str());
        btn_name->addClickEventListener([this, cfgId](Ref* sender)
        {
            ButtonEvent(cfgId);
        });

        if (pair->second.asInt() == 2)
        {
            btn_name->setTouchEnabled(false);
        }else
        {
            if (pair->second.asInt() == 0)
            {
                btn_name->getChildByName<ImageView*>("Sprite_New")->setVisible(true);
            }
            
            if (IsWorkHome(cfgId))
            {
                auto btn_Num = layout_Item->getChildByName<Button*>("Btn_Num");
                if (btn_Num != nullptr)
                {
                    btn_Num->setVisible(true);
                    auto level = dataRow.GetValue(CfgField::Level)->asInt();
                    btn_Num->setTitleText(StringUtils::format("%d", level));
                }
            }
            
            viv.clear();
            auto vmim = &dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
            CfgData::getInstance()->VMIMKToVIV_Sort(vmim, &viv);
            for (auto i = 0; i< (viv.size() > 3 ? 3 : viv.size()); i++)
            {
                langId = CfgData::getInstance()->GetValue(viv[i].first, CfgField::Name)->asInt();
                auto itemName = MultiLanguage::getInstance()->GetText(langId);
                auto text_name = StringUtils::format("Text_NameNum%d",i);
                auto text_value = StringUtils::format("%s x%d", itemName.c_str(), viv[i].second.asInt());
                layout_Item->getChildByName<Text*>(text_name)->setString(text_value.c_str());
                
                if(!UIUtils::getInstance()->isStuffEnough(viv[i].first, viv[i].second.asInt()))
                {
                    layout_Item->getChildByName<Text*>(text_name)->setColor(Color3B(128, 128, 128));
                }

            }
        }
        
        auto btn_Explain = layout_Item->getChildByName<Button*>("Btn_Explain");
        btn_Explain->addClickEventListener([=](Ref* sender)
           {
               CfgDataRow dataRow(cfgId);
               auto langId = dataRow.GetValue(CfgField::Details)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText);
           });
        
    }
    
    // 调整智能生产线的位置（从最后放到居住站下面）
    string key = StringUtils::format("%d", (int)CfgIdEnum::BuildItlProduceID);
    auto iter_produce = vm_MakeBuild->find(key);
    if (iter_produce != vm_MakeBuild->end()) {
        auto index = lView_List->getItems().size() - 1;
        auto item = lView_List->getItem(index);
        lView_List->removeItem(index);
        lView_List->insertCustomItem(item, 2);
        
        if (iter_produce->second.asInt() != 1) {
            // 说明已建 智能生产线
            
        }
        
    }
    lView_List->refreshView();
}

void MakeBuildUI::ButtonEvent(int cfgId)
{
    // 新手引导，建造仓库 - 主界面产生向右手势
    if(cfgId == (int)CfgIdEnum::StorehouseId)
        UIUtils::getInstance()->syncLeadHandShowUI("MainUI.csb", false);
    
    CfgDataRow dataRow(cfgId);
    vector<pair<int, int> > lackData;
    
    auto makeBuildVal = UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild);
    
    auto vm_MaterialID_M = dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
    
    bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vm_MaterialID_M, &lackData);
    
    if (delResutl)
    {
        //民居有很多级  100001001 ~ 100001040
        if (cfgId >= (int)CfgIdEnum::WorkerHouseMinId && cfgId <= (int)CfgIdEnum::WorkerHouseMaxId)
        {
            if (cfgId == (int)CfgIdEnum::WorkerHouseMaxId)
            {
                UserData::getInstance()->Update_VMI_Fun(&makeBuildVal->asValueMap(),
                                    cfgId , (int)MakeBuildStatus::MakeFinish, true);
            }else
            {
                UserData::getInstance()->Delete_VMI_Fun(&makeBuildVal->asValueMap(), cfgId);
                UserData::getInstance()->Update_VMI_Fun(&makeBuildVal->asValueMap(),
                                            cfgId + 1, (int)MakeBuildStatus::NewBuild, true);
            }
            
        }else
        {
            UserData::getInstance()->Update_VMI_Fun(&makeBuildVal->asValueMap(),
                                    cfgId, (int)MakeBuildStatus::MakeFinish, true);
        }
        
        // 建造黑市：赠送钻石，（首次登陆奖励）
        if (cfgId == (int)CfgIdEnum::ShopId) {
            MoveText::getInstance()->GetResText(m_RootWidget, (int)CfgIdEnum::GemId, 5);
            UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, (int)CfgIdEnum::GemId, 5);
            MsgListNode::getInstance()->NewMsg(LanguageIdEnum::BuildShopGetGem);
        }
        
        // 建造建筑从而达成的解锁事件，并执行该解锁事件
        UserEventContainer::getInstance()->Send(UserEventType::MakeBuildEvent, cfgId);
        UpdateUI();
        MsgListNode::getInstance()->NewMsg(cfgId, CfgField::Schedule);
        
    }else
    {
        // 弹出引导钻石消费界面
        int totalNum = UIUtils::getInstance()->getLackCostDiamond(&lackData);
        string showText = StringUtils::format(MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::Diamond_ToGet).c_str(), totalNum) + MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Name)->asInt());
        UIUtils::getInstance()->addLeadBuyLayer(this, showText, totalNum,
                                                [=]()
                                                {
                                                    UIUtils::getInstance()->costUpgNeededItem(&vm_MaterialID_M, &lackData);
                                                    
                                                    if (cfgId >= (int)CfgIdEnum::WorkerHouseMinId && cfgId <= (int)CfgIdEnum::WorkerHouseMaxId)
                                                    {
                                                        if (cfgId == (int)CfgIdEnum::WorkerHouseMaxId)
                                                        {
                                                            UserData::getInstance()->Update_VMI_Fun(&makeBuildVal->asValueMap(),
                                                                                                    cfgId , (int)MakeBuildStatus::MakeFinish, true);
                                                        }else
                                                        {
                                                            UserData::getInstance()->Delete_VMI_Fun(&makeBuildVal->asValueMap(), cfgId);
                                                            UserData::getInstance()->Update_VMI_Fun(&makeBuildVal->asValueMap(),
                                                                                                    cfgId + 1, (int)MakeBuildStatus::NewBuild, true);
                                                        }
                                                    }else
                                                    {
                                                        UserData::getInstance()->Update_VMI_Fun(&makeBuildVal->asValueMap(),
                                                                                                cfgId, (int)MakeBuildStatus::MakeFinish, true);
                                                    }
                                                    UserEventContainer::getInstance()->Send(UserEventType::MakeBuildEvent, cfgId);
                                                    UpdateUI();
                                                    MsgListNode::getInstance()->NewMsg(cfgId, CfgField::Schedule);
                                                });
        
        MoveText::getInstance()->LackResText(m_RootWidget, &lackData);
    }
}

//判断是否有建造民宅
bool MakeBuildUI::IsMakeWorkHome()
{
    auto buildId = 0;
    auto result = false;
    auto vm_makeBuild = UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild)->asValueMap();
    
    for (auto data : vm_makeBuild)
    {
        buildId = atoi(data.first.c_str());
        if (IsWorkHome(buildId))
        {
            result = true; break;
        }
    }
    
    return result;
}

//判断指定建筑是否民宅
bool MakeBuildUI::IsWorkHome(int buildId)
{
    auto result = false;
    if (buildId >= 100001001 && buildId <= 100001040)
    {
        result = true;
    }
    return  result;
}



