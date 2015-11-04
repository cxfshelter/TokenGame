//
//  MarketUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MarketUI.h"
#include "TiledMapUI.h"
#include "MapUI.h"

MarketUI* MarketUI::sm_MarketUI = nullptr;


MarketUI* MarketUI::getInstance()
{
    if (sm_MarketUI == nullptr)
    {
        sm_MarketUI = new (std::nothrow) MarketUI();
        if (sm_MarketUI->Init()) {}
    }
    return sm_MarketUI;
}

bool MarketUI::Init()
{
    return true;
}

void MarketUI::UnLoad()
{
    
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    TiledMapUI::getInstance()->SetTouchEnabled(true);
    
    UIBase::UnLoad();
}

void MarketUI::UpdateUI()
{
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Item_Market = layout_Template->getChildByName<Layout*>("Layout_Item");
    
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lView_List = layout_Center->getChildByName<ListView*>("LView_List");
    
    lView_List->removeAllChildren();
    
    CfgDataRow dataRow(m_BuildId);
    auto vm_item = &dataRow.GetValue(CfgField::OutputItemID_M)->asIntMultiKeyMap();
    
    auto itemId = 0;
    auto itemNum = 0;
    auto iter = vm_item->begin();
    while (iter != vm_item->end())
    {
        itemId = iter->first;
        itemNum = iter->second.asInt();
        iter++;
        
        auto layout_Item = layout_Item_Market->clone();
        CfgDataRow dataRow(itemId); //酒吧配置Id
        
        auto buyItemId = dataRow.GetValue(CfgField::ItemId)->asInt();
        
        if(UserData::getInstance()->Check_VI_Fun(UserDataEnum::OnlyItemList, Value(buyItemId))|| (MapUI::getInstance()->SetMarketBuyNum(m_BuildId, itemId, 0) <= 0) )
        {
            continue;
        }
        lView_List->addChild(layout_Item);
        
        CfgDataRow buyDataRow(buyItemId);
        auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        auto langId = buyDataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        
        btn_Name->setTitleText(langText);
        btn_Name->addClickEventListener([=](Ref* sender)
        {
            
            CfgDataRow dataRow(itemId);
            auto vm_CostItem = dataRow.GetValue(CfgField::CostItemID_M)->asIntMultiKeyMap();
            
            int itemRealId = dataRow.GetValue(CfgField::ItemId)->asInt();
            CfgDataRow itemRow(itemRealId);
            
            vector<pair<int, int> > lackData;
            bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vm_CostItem, &lackData);
            if (delResutl)
            {
                auto onlyItem = itemRow.GetValue(CfgField::OnlyType)->asInt();
                if (onlyItem == 1)
                {
                    UserData::getInstance()->Add_VI_Fun(UserDataEnum::OnlyItemList, buyItemId);
                }
                if (buyItemId / 1000000 == 205)
                {
                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::LiveHero, buyItemId, 1);
                }else
                {
                    UserData::getInstance()->Update_VMI_Fun(UserDataEnum::BackPack, buyItemId, 1);
                }
                MapUI::getInstance()->SetMarketBuyNum(m_BuildId, itemId, -1);
                UpdateUI();
                
                CfgDataRow dataRow(buyItemId);
                auto langId = dataRow.GetValue(CfgField::Details)->asInt();
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                langText = StringUtils::format(langText.c_str(), 1);
                MsgListNode::getInstance()->NewMsg(langText);
                
            }else
            {
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
               CfgDataRow dataRow(itemId);
               auto itemID2 = dataRow.GetValue(CfgField::ItemId)->asInt();
               CfgDataRow dataRow2(itemID2);
               auto langId = dataRow2.GetValue(CfgField::Details)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText);
               
           });
    }
}

void MarketUI::LoadFinish()
{
    m_BuildId = m_LoadPram.asInt();
    TiledMapUI::getInstance()->SetTouchEnabled(false);
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto btn_Left = layout_title->getChildByName<Button*>("Btn_Left");
    
    btn_Left->addClickEventListener([this](Ref* sender)
    {
        UnLoad();
        
    });
}


