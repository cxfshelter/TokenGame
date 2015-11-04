//
//  TalentUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "TalentUI.h"
#include "BattlePubScene.h"


TalentUI* TalentUI::sm_TalentUI = nullptr;

TalentUI* TalentUI::getInstance()
{
    if (sm_TalentUI == nullptr)
    {
        sm_TalentUI = new (std::nothrow) TalentUI();
        if (sm_TalentUI->Init()) {}
    }
    return sm_TalentUI;
}

bool TalentUI::Init()
{
    return true;
}

void TalentUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    ProductionItem::getInstance()->UnSchedule();
    UIBase::UnLoad();
}

void TalentUI::UpdateUI()
{
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Template_Item = layout_Template->getChildByName<Layout*>("Layout_Item");
    
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lview_List =layout_Center->getChildByName<ListView*>("LView_List");

    auto talentData = &UserData::getInstance()->GetUserData(UserDataEnum::TalentList)->asValueVector();

    for (auto iter = talentData->begin(); iter != talentData->end(); iter++)
    {
        auto cfgId = iter->asInt();
        auto layout_Item = layout_Template_Item->clone();
        lview_List->addChild(layout_Item);
        auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        
        CfgDataRow dataRow(cfgId);
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Name->setTitleText(langText);
        btn_Name->addClickEventListener([this, cfgId](Ref* sender)
            {
                auto langId = CfgData::getInstance()->GetValue(cfgId, CfgField::Details)->asInt();
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText);
            });

        auto text_Desc = layout_Item->getChildByName<Text*>("Text_Desc");
        langId = dataRow.GetValue(CfgField::Details)->asInt();
        langText = MultiLanguage::getInstance()->GetText(langId);
        text_Desc->setString(langText);
        
        
        auto btn_Explain =  layout_Item->getChildByName<Button*>("Btn_Explain");
        
        btn_Explain->addClickEventListener([this, cfgId](Ref* sender)
           {
               CfgDataRow dataRow(cfgId);

               auto langId = CfgData::getInstance()->GetValue(cfgId, CfgField::Details)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText);
           });
        
        
    }
    
}

void TalentUI::LoadFinish()
{
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    ProductionItem::getInstance()->SetSchedule(m_RootWidget);
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto btn_Right = layout_title->getChildByName<Button*>("Btn_Right");
    
    btn_Right->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
        auto scene = BattlePubScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    });
    
    UIUtils::getInstance()->addSlideEvent(m_RootWidget,
                                          [this]()
                                          {
                                              this->UnLoad();
                                              auto scene = BattlePubScene::createScene();
                                              Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                          }
                                          ,nullptr
                                          );

}


