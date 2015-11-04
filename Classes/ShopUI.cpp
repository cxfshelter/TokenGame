//
//  ShopUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "ShopUI.h"
#include "MainScene.h"


ShopUI* ShopUI::sm_ShopUI = nullptr;


ShopUI* ShopUI::getInstance()
{
    if (sm_ShopUI == nullptr)
    {
        sm_ShopUI = new (std::nothrow) ShopUI();
        if (sm_ShopUI->Init()) {}
    }
    return sm_ShopUI;
}

bool ShopUI::Init()
{
    return true;
}

void ShopUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    ProductionItem::getInstance()->UnSchedule();
    
    ValueMap* vm_ShopItem =  &(UserData::getInstance()->GetUserData(UserDataEnum::ShopItem)->asValueMap());
    
    for (auto pair = vm_ShopItem->begin(); pair != vm_ShopItem->end(); pair++)
    {
        if (pair->second.asInt() == true)
        {
            pair->second = Value(false);
            //UserData::getInstance()->Update_VMS_Fun(vm_ShopItem, pair->first , false);
        }
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    UIBase::UnLoad();
}

void ShopUI::UpdateUI()
{
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Item_Template = layout_Template->getChildByName<Layout*>("Layout_Item");
    
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lView_List = layout_Center->getChildByName<ListView*>("LView_List");
    
    ValueMap* vm_ShopItem =  &(UserData::getInstance()->GetUserData(UserDataEnum::ShopItem)->asValueMap());
    
    int cfgId;
    int langId;
    string priceText;
    
    Button* btn_Name;
    Button*btn_Explain;
    Text* text_NameNum;
    string langText;
    
    const ValueMapIntKey* vmi;
    
    vector<pair<int, Value>> viv_ShopItem;
    UserData::getInstance()->VMSKToVIV_Sort(vm_ShopItem, &viv_ShopItem);
    lView_List->removeAllChildren();
    for (auto iter = viv_ShopItem.begin(); iter != viv_ShopItem.end(); iter++)
    {
        
        cfgId = iter->first;
        if(UserData::getInstance()->Check_VI_Fun(UserDataEnum::OnlyItemList, Value(cfgId)))
        {
            continue;
        }
        
        CfgDataRow dataRow(cfgId);

        auto layout_Item = layout_Item_Template->clone();
        lView_List->addChild(layout_Item);

        btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        auto text_Name = btn_Name->getChildByName<Text*>("Text_Name");
        
        langId = dataRow.GetValue(CfgField::Name)->asInt();
        langText = MultiLanguage::getInstance()->GetText(langId);
        text_Name->setString(langText);
        
        auto newSprite = btn_Name->getChildByName<ImageView*>("Sprite_New");
        if (newSprite != nullptr) newSprite->setVisible(iter->second.asBool());
        
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
        
        UIUtils::getInstance()->addBuyBatches(btn_Name, this, cfgId);
        btn_Name->addClickEventListener([this, cfgId, btn_Name](Ref* sender)
        {
            if (btn_Name->getTag() == BTN_IS_LONGBTN) {
                btn_Name->setTag(BTN_NOT_LONGBTN);
                return;
            }
            
            UIUtils::getInstance()->handleBuy(cfgId, 1, this);
            UpdateUI();
        });
        
        text_NameNum = layout_Item->getChildByName<Text*>("Text_NameNum");
        auto vmim_Price = &dataRow.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap();
        for (auto iter_price = vmim_Price->begin(); iter_price != vmim_Price->end(); iter_price++)
        {
            if (iter_price->first == (int)CfgIdEnum::GemId)
            {
                
            }else
            {
                CfgDataRow dataRow(iter_price->first);
                vmi = CfgData::getInstance()->GetCfgRow(iter_price->first);
                langId = dataRow.GetValue(CfgField::Name)->asInt();
                langText = MultiLanguage::getInstance()->GetText(langId);
                priceText = StringUtils::format("%s x%d", langText.c_str(), iter_price->second.asInt());
                
                if(!UIUtils::getInstance()->isStuffEnough(iter_price->first, iter_price->second.asInt()))
                {
                    // 数量不足，置灰
                    text_NameNum->setColor(Color3B(128, 128, 128));
            }
        }
        text_NameNum->setString(priceText);
        
        btn_Explain = layout_Item->getChildByName<Button*>("Btn_Explain");
        btn_Explain->addClickEventListener(
           [this, cfgId](Ref* sender)
           {
               auto langId = CfgData::getInstance()->GetValue(cfgId, CfgField::Details)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText);
           });
        }
    }
}

void ShopUI::LoadFinish()
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

