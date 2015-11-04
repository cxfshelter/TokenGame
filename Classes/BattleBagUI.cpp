//
//  BattleBagUI.cpp
//  TokenGame
//
//  Created by best on 15/5/6.
//
//

#include "BattleBagUI.h"
#include "MapScene.h"

BattleBagUI* BattleBagUI::sm_BattleBagUI = nullptr;

BattleBagUI* BattleBagUI::getInstance()
{
    if (sm_BattleBagUI == nullptr) {
        sm_BattleBagUI = new (std::nothrow) BattleBagUI();
        if (sm_BattleBagUI->Init()) { }
    }
    return sm_BattleBagUI;
    
}

bool BattleBagUI::Init()
{
    return true;
}

void BattleBagUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    ProductionItem::getInstance()->UnSchedule();
    UIBase::UnLoad();
}

void BattleBagUI::UpdateUI()
{
    auto layout_Template = m_RootWidget->getChildByName<Layout *>("Layout_Template");
    auto layout_Option = layout_Template->getChildByName<Layout *>("Layout_Option");
    auto lview_List = m_RootWidget->getChildByName<ListView *>("LView_BagItem");
    
    int cfgId;
    // 添加背包仓库item
    auto vm_Shorehouse = &UserData::getInstance()->GetUserData(UserDataEnum::Storehouse)->asValueMap();
    for (auto iter = vm_Shorehouse->begin(); iter != vm_Shorehouse->end(); iter++)
    {
        cfgId = atoi(iter->first.c_str());
        CfgDataRow dataRow(cfgId);
        if (dataRow.GetValue(CfgField::Expedition)->asInt() != 1) {
            continue;
        }
        
        auto layout_Item = layout_Option->clone();
        lview_List->addChild(layout_Item);
        
        auto btn_Name = layout_Item->getChildByName<Button *>("Btn_Name");
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
        auto btn_explain = layout_Item->getChildByName<Button *>("Btn_Explain");
        btn_explain->addClickEventListener([=](Ref* sender)
            {
                CfgDataRow dataRow(cfgId);
                auto langId = dataRow.GetValue(CfgField::Details)->asInt();
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText, cfgId);
            });
        
        auto level = dataRow.GetValue(CfgField::Level)->asInt();
        auto layout_Start = layout_Item->getChildByName<Layout *>("Start");
        if (layout_Start != nullptr) {
            for (auto i = 1; i <= level; ++i) {
                auto startName = StringUtils::format("Start_%d", i-1);
                auto imgStart = layout_Start->getChildByName<ImageView *>(startName);
                if (imgStart != nullptr) {
                    imgStart->setVisible(true);
                }
            }
        }
        
        auto backPackNum = 0;
        auto vm_BackPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
        auto val_BackPack = UserData::getInstance()->Get_MVI_Value(vm_BackPack, cfgId);
        if (val_BackPack != Value::Null) {
            backPackNum = val_BackPack.asInt();
        }
        auto text_Num = layout_Item->getChildByName<Text *>("Text_Num");
        text_Num->setString(StringUtils::format("%d", backPackNum));
        
    }
    
    // 添加背包人物item
    auto vm_LiveHeo = &UserData::getInstance()->GetUserData(UserDataEnum::LiveHero)->asValueMap();
    for (auto iter = vm_LiveHeo->begin(); iter != vm_LiveHeo->end(); ++iter) {
        cfgId = atoi(iter->first.c_str());
        CfgDataRow dataRow(cfgId);
        auto liveHeroNum = 0;
        auto vm_BackPack = &UserData::getInstance()->GetUserData(UserDataEnum::BackPack)->asValueMap();
        auto val_BackPack = UserData::getInstance()->Get_MVI_Value(vm_BackPack, cfgId);
        if (val_BackPack != Value::Null) {
            liveHeroNum = val_BackPack.asInt();
        }
        if (liveHeroNum <= 0) {
            // 判断是否在背包上
            continue;
        }
        
        auto layout_Item = layout_Option->clone();
        lview_List->addChild(layout_Item);
        
        auto btn_Name = layout_Item->getChildByName<Button *>("Btn_Name");
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
        auto btn_explain = layout_Item->getChildByName<Button *>("Btn_Explain");
        btn_explain->addClickEventListener([=](Ref* sender)
           {
               CfgDataRow dataRow(cfgId);
               auto langId = dataRow.GetValue(CfgField::Details)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText, cfgId);
           });
        
        auto level = dataRow.GetValue(CfgField::Level)->asInt();
        auto layout_Start = layout_Item->getChildByName<Layout *>("Start");
        for (auto i = 1; i <= level; ++i) {
            auto startName = StringUtils::format("Start_%d", i - 1);
            auto imgStart = layout_Start->getChildByName<ImageView *>(startName);
            if (imgStart != nullptr) {
                imgStart->setVisible(true);
            }
        }
        auto text_Num = layout_Item->getChildByName<Text *>("Text_Num");
        text_Num->setString(StringUtils::format("%d", liveHeroNum));
        
    }

}

void BattleBagUI::LoadFinish()
{
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    ProductionItem::getInstance()->SetSchedule(m_RootWidget);
    
    auto layout_title = m_RootWidget->getChildByName<Layout *>("Layout_Title");
    auto btn_Left = layout_title->getChildByName<Button *>("Btn_Left");
    btn_Left->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
        auto scene = MapScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    });
    
    UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                          [this]()
                                          {
                                              this->UnLoad();
                                              auto scene = MapScene::createScene();
                                              Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                          }, nullptr);
}



