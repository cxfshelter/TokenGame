//
//  WineBarUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "WineBarUI.h"
#include "TiledMapUI.h"

WineBarUI* WineBarUI::sm_WineBarUI = nullptr;


WineBarUI* WineBarUI::getInstance()
{
    if (sm_WineBarUI == nullptr)
    {
        sm_WineBarUI = new (std::nothrow) WineBarUI();
        if (sm_WineBarUI->Init()) {}
    }
    return sm_WineBarUI;
}

bool WineBarUI::Init()
{
    return true;
}

void WineBarUI::UnLoad()
{
    
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    TiledMapUI::getInstance()->SetTouchEnabled(true);
    
    UIBase::UnLoad();
}


void WineBarUI::UpdateUI()
{
    auto layout_Tempalte = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_ItemTemplate = layout_Tempalte->getChildByName<Layout*>("Layout_Item");
    
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lView_List = layout_Center->getChildByName<ListView*>("LView_List");
    lView_List->removeAllChildren();
    
    CfgDataRow dataRow(m_BuildId);
    auto vm_item = &dataRow.GetValue(CfgField::OutputItemID_M)->asIntMultiKeyMap();
    
    auto itemId = 0; //酒吧配置Id
    auto itemNum = 0;
    
    auto iter = vm_item->begin();
    while (iter != vm_item->end())
    {
        
        itemId = iter->first;
        itemNum = iter->second.asInt(); iter++;
        
        auto layout_Item = layout_ItemTemplate->clone();
        lView_List->addChild(layout_Item);
        
        CfgDataRow dataRow(itemId); //酒吧配置Id
        
        auto heroId = dataRow.GetValue(CfgField::ItemId)->asInt();
        
        if(UserData::getInstance()->Check_VI_Fun(UserDataEnum::OnlyItemList, Value(heroId)))
        {
            continue;
        }
        
        CfgDataRow heroDataRow(heroId);
        
        auto level = heroDataRow.GetValue(CfgField::Level)->asInt();
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
        
        auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        auto langId = heroDataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Name->setTitleText(langText);
        
        btn_Name->addClickEventListener([=](Ref* sender)
        {
            CfgDataRow dataRow(itemId); //酒吧配置Id
            auto vm_CostItem = dataRow.GetValue(CfgField::CostItemID_M)->asIntMultiKeyMap();
            vector<pair<int, int> > lackData;
            bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vm_CostItem, &lackData);
            if (delResutl)
            {
                auto onlyItem = dataRow.GetValue(CfgField::OnlyType)->asInt();
                if (onlyItem == 1)
                {
                    UserData::getInstance()->Add_VI_Fun(UserDataEnum::OnlyItemList, heroId);
                }
                if (heroId / 1000000 == 205)
                {
                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::LiveHero, heroId, 1);
                    
                }else
                {
                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::BackPack, heroId, 1);
                    
                }
                
                UpdateUI();
                
                CfgDataRow dataRow(heroId);
                auto langId = dataRow.GetValue(CfgField::Details)->asInt();
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                langText = StringUtils::format(langText.c_str(), 1);
                MsgListNode::getInstance()->NewMsg(langText);
                
            }else
            {
                MoveText::getInstance()->LackResText(m_RootWidget, &lackData);
            }
            
        });
        
        auto vm_CostItem = &dataRow.GetValue(CfgField::CostItemID_M)->asIntMultiKeyMap();
        auto iter_CostItem = vm_CostItem->begin();
        if (iter_CostItem != vm_CostItem->end())
        {
            auto text_NameNum = layout_Item->getChildByName<Text*>("Text_NameNum");
            
            auto costItemID = iter_CostItem->first;
            
            auto costItemNum = iter_CostItem->second.asInt();
            
            CfgDataRow CostItemDataRow( costItemID);
            langId = CostItemDataRow.GetValue(CfgField::Name)->asInt();
            langText = MultiLanguage::getInstance()->GetText(langId);
            langText = StringUtils::format("%s x%d", langText.c_str(), costItemNum);
            text_NameNum->setString(langText);
            
        }
        
        auto btn_Explain = layout_Item->getChildByName<Button*>("Btn_Explain");
        btn_Explain->addClickEventListener([=](Ref* sender)
        {
            CfgDataRow dataRow(heroId);
            auto langId = dataRow.GetValue(CfgField::Details)->asInt();
            auto langText = MultiLanguage::getInstance()->GetText(langId);
            TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText, heroId);
        });
        
        UIUtils::getInstance()->showProperImg(btn_Name, heroId);
        
    }
    
}

void WineBarUI::LoadFinish()
{
    m_BuildId = (uint32_t)(m_LoadPram.asInt());
    TiledMapUI::getInstance()->SetTouchEnabled(false);
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto btn_Left = layout_title->getChildByName<Button*>("Btn_Left");
    
    btn_Left->addClickEventListener([this](Ref* sender)
    {
        
        UnLoad();
    });
}


