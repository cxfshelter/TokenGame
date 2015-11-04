//
//  MakeItemUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MainScene.h"
#include "MakeItemUI.h"

MakeItemUI* MakeItemUI::sm_MakeItemUI = nullptr;


MakeItemUI* MakeItemUI::getInstance()
{
    if (sm_MakeItemUI == nullptr)
    {
        sm_MakeItemUI = new (std::nothrow) MakeItemUI();
        if (sm_MakeItemUI->Init()) {}
    }
    return sm_MakeItemUI;
}

bool MakeItemUI::Init()
{

    return true;
}


void MakeItemUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    ProductionItem::getInstance()->UnSchedule();
    ValueMap* vm_MakeItem =  &(UserData::getInstance()->GetUserData(UserDataEnum::MakeItem)->asValueMap());
    
    for (auto pair = vm_MakeItem->begin(); pair != vm_MakeItem->end(); pair++)
    {
        if (pair->second.asBool() == true)
        {
            pair->second = Value(false);
            //UserData::getInstance()->Update_VMS_Fun(vm_MakeItem, pair->first , false);
        }
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    UIBase::UnLoad();
}

void MakeItemUI::makeItemEvent(int cfgId)
{
    CfgDataRow dataRow(cfgId);
    if(dataRow.GetValue(CfgField::UpGradable)->asInt() == 1)
    {
        UserData::getInstance()->Delete_VMI_Fun(UserDataEnum::MakeItem, cfgId);
        auto nextCfgId = cfgId + 1;
        
        auto nextCfgIdData = CfgData::getInstance()->GetCfgRow(nextCfgId);
        if (nextCfgIdData != nullptr)
        {
            auto makeItem = &UserData::getInstance()->GetUserData(UserDataEnum::MakeItem)->asValueMap();
            string strCfgId = StringUtils::format("%d", nextCfgId);
            if (makeItem->find(strCfgId) == makeItem->end())
            {
                makeItem->insert(make_pair(strCfgId, Value(true)));
                //                            UserData::getInstance()->SaveUserData();
                UserData::getInstance()->SaveCorUserData(false);
            }
            
        }
    }else
    {
        if (dataRow.GetValue(CfgField::Uniqueness)->asInt() == 1)
        {
            UserData::getInstance()->Add_VI_Fun(UserDataEnum::OnlyItemList, cfgId);
        }
        UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, cfgId, 1);
    }
    
    UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, cfgId);
    
    int iter_fun_Val1 , iter_fun_Val2;
    auto vmi_function = dataRow.GetValue(CfgField::FunctionID_M)->asIntMultiKeyMap();
    if (!vmi_function.empty())
    {
        auto iter_fun = vmi_function.begin();
        auto cfgIdHead = iter_fun->first / 100;
        auto val = iter_fun->second.asInt();
        switch (cfgIdHead)
        {
            case 1000020: //背包容量
                UserData::getInstance()->Update_Value(UserDataEnum::BackPackMaxNum, Value(val));
                break;
            case 1000021: //队伍人数
                UserData::getInstance()->Update_Value(UserDataEnum::RanksMaxNum, Value(val));
                break;
            case 1000022: //炼金数量
                UserData::getInstance()->Update_Value(UserDataEnum::AlchemyGoldNum, Value(val));
                break;
            case 1000023: //采集数量
                iter_fun_Val1 = iter_fun->second.asInt();
                iter_fun++;
                if (iter_fun == vmi_function.end())
                {
                    CCLOGERROR("CfgId:%d FunctionID_M Error", cfgId);
                }else
                {
                    iter_fun_Val2 = iter_fun->second.asInt();
                    if (iter_fun_Val1 < iter_fun_Val2)
                    {
                        UserData::getInstance()->Update_Value(UserDataEnum::CollectMinNum, Value(iter_fun_Val1) );
                        UserData::getInstance()->Update_Value(UserDataEnum::CollectMaxNum, Value(iter_fun_Val2) );
                    }else
                    {
                        UserData::getInstance()->Update_Value(UserDataEnum::CollectMinNum, Value(iter_fun_Val2));
                        UserData::getInstance()->Update_Value(UserDataEnum::CollectMaxNum, Value(iter_fun_Val1));
                    }
                }
                break;
            case 1000000: //制作获取金币数量
            {
                auto itemId = iter_fun->first;
                auto itemNum = iter_fun->second.asInt();
                MoveText::getInstance()->GetResText(m_RootWidget, itemId, itemNum);
                UserData::getInstance()->Update_VMI_Fun(UserDataEnum::Storehouse, itemId, itemNum);
                break;
            }
            default:
                break;
        }
    }
}

void MakeItemUI::UpdateUI()
{
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    //有新建筑可建或有建筑可建
    auto layout_Item_Template = layout_Template->getChildByName<Layout*>("Layout_Item");
    
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lView_List = layout_Center->getChildByName<ListView*>("LView_List");
    
    vector<pair<int, Value>> viv;
    //每次清空 子控件
    lView_List->removeAllChildren();

    vector<pair<int, Value>> viv_MakeItem;
    ValueMap* vm_MakeItem =  &(UserData::getInstance()->GetUserData(UserDataEnum::MakeItem)->asValueMap());
    UserData::getInstance()->VMSKToVIV_Sort(vm_MakeItem, &viv_MakeItem);
    
    for (auto iter = viv_MakeItem.begin(); iter != viv_MakeItem.end(); iter++)
    {
        auto cfgId = iter->first;
        if(UserData::getInstance()->Check_VI_Fun(UserDataEnum::OnlyItemList, Value(cfgId)))
        {
            continue;
        }
        
        auto layout_Item = layout_Item_Template->clone();
        layout_Item->setVisible(true);
        lView_List->addChild(layout_Item);
        CfgDataRow dataRow(cfgId);
        
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        
        auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        btn_Name->setTitleText(langText.c_str());
        btn_Name->getChildByName<ImageView*>("Sprite_New")->setVisible(iter->second.asBool());
        UIUtils::getInstance()->addMakeBatches(btn_Name, this, cfgId);
        btn_Name->addClickEventListener([this, cfgId, btn_Name](Ref* sender)
        {
            if (btn_Name->getTag() == BTN_IS_LONGBTN) {
                btn_Name->setTag(BTN_NOT_LONGBTN);
                return;
            }
            
            CfgDataRow dataRow(cfgId);
            auto vmi_Price = dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
            
            vector<pair<int, int> > lackData;
            bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vmi_Price, &lackData);
            if (delResutl)
            {
                makeItemEvent(cfgId);
                MsgListNode::getInstance()->NewMsg(cfgId, 1, CfgField::Schedule);
                UpdateUI();
            }else
            {
                int totalNum = UIUtils::getInstance()->getLackCostDiamond(&lackData);
                string showText = StringUtils::format(MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::Diamond_ToGet).c_str(), totalNum) + MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Name)->asInt());
                UIUtils::getInstance()->addLeadBuyLayer(this, showText, totalNum,
                                                        [=]()
                                                        {
                                                            UIUtils::getInstance()->costUpgNeededItem(&vmi_Price, &lackData);
                                                            makeItemEvent(cfgId);
                                                            MsgListNode::getInstance()->NewMsg(cfgId, 1, CfgField::Schedule);
                                                            UpdateUI();
                                                        });
                
                MoveText::getInstance()->LackResText(m_RootWidget, &lackData);
            }
            
        });
        
        auto level = dataRow.GetValue(CfgField::Level)->asInt();
        auto layout_Start = layout_Item->getChildByName<Layout*>("Start");
        if (layout_Start != nullptr)
        {
            for (auto i = 1; i <= level; i++)
            {
                auto startName = StringUtils::format("Start_%d", i - 1);
                auto imgStart = layout_Start->getChildByName<ImageView*>(startName);
                if (imgStart != nullptr) imgStart->setVisible(true);
            }
        }

        viv.clear();
        auto vmim = dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
        CfgData::getInstance()->VMIMKToVIV_Sort(&vmim, &viv);
        for (auto i = 0; i < (viv.size() > 3 ? 3 : viv.size()); i++)
        {
            auto textName = StringUtils::format("Text_NameNum%d",i);
            auto text_NameNum = layout_Item->getChildByName<Text*>(textName);
            langId = CfgData::getInstance()->GetValue(viv[i].first, CfgField::Name)->asInt();
            langText = MultiLanguage::getInstance()->GetText(langId);
            
            text_NameNum->setString(StringUtils::format("%s x%d", langText.c_str(), viv[i].second.asInt()));
            if(!UIUtils::getInstance()->isStuffEnough(viv[i].first, viv[i].second.asInt()))
            {
                // 数量不足，置灰
                text_NameNum->setColor(Color3B(128, 128, 128));
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
    lView_List->refreshView();
}

void MakeItemUI::LoadFinish()
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


