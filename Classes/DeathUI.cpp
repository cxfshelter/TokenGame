//
//  DeathUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "DeathUI.h"
#include "TiledMapUI.h"
#include "AchievementSystem.h"

DeathUI* DeathUI::sm_DeathUI = nullptr;


DeathUI* DeathUI::getInstance()
{
    if (sm_DeathUI == nullptr)
    {
        sm_DeathUI = new (std::nothrow) DeathUI();
        if (sm_DeathUI->Init()) {}
    }
    return sm_DeathUI;
}

bool DeathUI::Init()
{
    return true;
}

void DeathUI::UnLoad()
{
    
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    TiledMapUI::getInstance()->SetTouchEnabled(true);
    UIBase::UnLoad();
}

void DeathUI::UpdateUI()
{
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Item = layout_Template->getChildByName<Layout*>("Layout_Item");
    
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lView_List = layout_Center->getChildByName<ListView*>("LView_List");
    lView_List->removeAllChildren();
    auto vm_DeathHero = &UserData::getInstance()->GetUserData(UserDataEnum::DeathHero)->asValueMap();
    for (auto iter = vm_DeathHero->begin(); iter != vm_DeathHero->end(); iter++)
    {
        auto heroId =  atoi(iter->first.c_str());
        auto heroNum = iter->second.asInt();
        
        CfgDataRow dataRow(heroId);
        
        auto layout = layout_Item->clone();
        lView_List->addChild(layout);
        
        auto btn_Name = layout->getChildByName<Button*>("Btn_Name");
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Name->setTitleText(langText);
        btn_Name->addClickEventListener([=](Ref* sender)
            {
                ShowOptionDialog(heroId);
            });
        
        
        auto text_Num = layout->getChildByName<Text*>("Text_Num");
        
        auto text_Skill = layout->getChildByName<Text*>("Text_Skill");
        
        text_Num->setString(StringUtils::format("x%d", heroNum));
        langId = (int)LanguageIdEnum::SkillId;
        langText = MultiLanguage::getInstance()->GetText(langId);
        
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
        
        auto vi_skill = dataRow.GetValue(CfgField::SkillID_V)->asValueVector();
        auto iter_skill = vi_skill.begin();
        auto skillId = iter_skill->asInt();
        CfgDataRow dataRow_Skill(skillId);
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SkillId);
        langText += MultiLanguage::getInstance()->GetText(dataRow_Skill.GetValue(CfgField::Name)->asInt());
        text_Skill->setString(langText);
        
        
        auto text_Attribute = layout->getChildByName<Text*>("Text_Attribute");
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::LifeId);
        langText += MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Hp)->asInt());
        
        langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::PowerId);
        langText += MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Atk)->asInt());
        
        langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SpeedId);
        auto speed = dataRow.GetValue(CfgField::Spd_F)->asFloat();
        langText += MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Spd_F)->asInt());
        text_Attribute->setString(StringUtils::format("%s%g", langText.c_str(), speed));
        
        auto btn_Explain = layout->getChildByName<Button*>("Btn_Explain");
        btn_Explain->addClickEventListener([=](Ref* sender)
           {
               CfgDataRow dataRow(heroId);
               auto langId = dataRow.GetValue(CfgField::Details)->asInt();
               auto langText = MultiLanguage::getInstance()->GetText(langId);
               TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText, heroId);
           });
        
        UIUtils::getInstance()->showProperImg(btn_Name, heroId);
        
    }
    
    auto text_firstDesc = layout_Center->getChildByName<Text *>("Text_FirstDesc");
    // 墓园列表无内容
    if (lView_List->getItems().size() <= 0) {
        text_firstDesc->setVisible(true);
        
        CfgDataRow dataRow((int)CfgIdEnum::MapDeadBuildId);
        auto langId = dataRow.GetValue(CfgField::BodescribeId)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        text_firstDesc->setString(langText);
    }
    else
    {
        text_firstDesc->setVisible(false);
    }
}

void DeathUI::ShowOptionDialog(int heroId)
{
    auto layout_Dialog = m_RootWidget->getChildByName<Layout*>("Layout_OptionDialog");
    auto text_Title = layout_Dialog->getChildByName<Text*>("Text_Title");
    layout_Dialog->setVisible(true);
    
    auto langId = (int)LanguageIdEnum::DifficultyOption;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    text_Title->setString(langText);
    
    auto text_Desc = layout_Dialog->getChildByName<Text*>("Text_Desc");
    langId = (int)LanguageIdEnum::HowToDealLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_Desc->setString(langText);
    
    
    auto btn_Revive = layout_Dialog->getChildByName<Button*>("Btn_Revive");
    langId = (int)LanguageIdEnum::ReviveLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Revive->setTitleText(langText);
    
    auto btn_Cancel = layout_Dialog->getChildByName<Button*>("Btn_Cancel");
    langId = (int)LanguageIdEnum::CancelLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Cancel->setTitleText(langText);

    auto btn_Bury = layout_Dialog->getChildByName<Button*>("Btn_Bury");
    langId = (int)LanguageIdEnum::BuryLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Bury->setTitleText(langText);

    auto text_Text = btn_Bury->getChildByName<Text*>("Text_Text");
    langId = (int)LanguageIdEnum::DeadPeace;
    langText = MultiLanguage::getInstance()->GetText(langId);
    text_Text->setString(langText);

    auto heroLv = heroId % 10;
    auto cfgId = (int)CfgIdEnum::ReviveHero + heroLv;

    auto text_Item = btn_Revive->getChildByName<Text*>("Text_Item");
    langId = (int)LanguageIdEnum::ConsumeLangid;
    langText = MultiLanguage::getInstance()->GetText(langId);
    
    
    CfgDataRow dataRow(cfgId);
    auto mv_ReviveItem = &dataRow.GetValue(CfgField::ReproItemID_M)->asIntMultiKeyMap();
    auto iterReviveItem = mv_ReviveItem->begin();
    
    CfgDataRow itemDataRow(iterReviveItem->first);
    
    langId = itemDataRow.GetValue(CfgField::Name)->asInt();
    auto itemName = MultiLanguage::getInstance()->GetText(langId);
    auto itemNum = iterReviveItem->second.asInt();
    langText = StringUtils::format("%s %sx%d", langText.c_str(), itemName.c_str(), itemNum);
    text_Item->setString(langText);
    
    
    btn_Revive->addClickEventListener([=](Ref* sender)
      {
          CfgDataRow dataRow(cfgId);
          auto mv_ReviveItem = &dataRow.GetValue(CfgField::ReproItemID_M)->asIntMultiKeyMap();
          vector<pair<int, int> > lackData;
          bool delResutl = UserData::getInstance()->DeleteStorehouseData(mv_ReviveItem , &lackData);
          if (delResutl)
          {
              auto vm_live = UserData::getInstance()->GetUserData(UserDataEnum::LiveHero);
              UserData::getInstance()->Update_VMI_Fun(&vm_live->asValueMap(), heroId, 1);
              
              auto vm_Death = UserData::getInstance()->GetUserData(UserDataEnum::DeathHero);
              UserData::getInstance()->Update_VMI_Fun(&vm_Death->asValueMap(), heroId, -1);
              
              layout_Dialog->setVisible(false);   UpdateUI();
              
              auto iter = mv_ReviveItem->begin();
              while (iter != mv_ReviveItem->end())
              {
                  auto itemId = iter->first;
                  auto itemNum = iter->second.asInt(); iter++;
                  CfgDataRow itemDataRow(itemId);
                  auto langId = (int) LanguageIdEnum::ReviveDescLangId;
                  auto langText = MultiLanguage::getInstance()->GetText(langId);
                  
                  langId = itemDataRow.GetValue(CfgField::Name)->asInt();
                  langText += MultiLanguage::getInstance()->GetText(langId);
                  langText = StringUtils::format("%s x%d ", langText.c_str(), itemNum);
                  MsgListNode::getInstance()->NewMsg(langText);
              }
          }else
          {
              layout_Dialog->setVisible(false);
              MoveText::getInstance()->LackResText(m_RootWidget, &lackData);
          }
          
      });
    
    btn_Bury->addClickEventListener([=](Ref* sender)
        {
            auto vm_Death = UserData::getInstance()->GetUserData(UserDataEnum::DeathHero);
            UserData::getInstance()->Update_VMI_Fun(&vm_Death->asValueMap(), heroId, -1);

            CfgDataRow dataRow(cfgId);
            auto mv_BuryItem = &dataRow.GetValue(CfgField::BuryItemID_M)->asIntMultiKeyMap();
            UserData::getInstance()->AddStorehouseData(mv_BuryItem);
            layout_Dialog->setVisible(false);   UpdateUI();
            
            auto level = dataRow.GetValue(CfgField::Level)->asInt();
            AchievementSystem::getInstance()->CheckAchievement(UserEventType::BuryHero, level);
            
            auto iter = mv_BuryItem->begin();
            while (iter != mv_BuryItem->end())
            {
                auto itemId = iter->first;
                auto itemNum = iter->second.asInt(); iter++;
                CfgDataRow itemDataRow(itemId);
                auto langId = (int) LanguageIdEnum::BuryDescLangId;
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                
                langId = itemDataRow.GetValue(CfgField::Name)->asInt();
                langText += MultiLanguage::getInstance()->GetText(langId);
                langText = StringUtils::format("%s x%d ", langText.c_str(), itemNum);
                MsgListNode::getInstance()->NewMsg(langText);
                
            }
    });
    
    btn_Cancel->addClickEventListener([=](Ref* sender)
      {
          layout_Dialog->setVisible(false);
      });
    
}

void DeathUI::LoadFinish()
{
    TiledMapUI::getInstance()->SetTouchEnabled(false);
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto btn_Left = layout_title->getChildByName<Button*>("Btn_Left");
    
    btn_Left->addClickEventListener([this](Ref* sender)
    {
        this->UnLoad();
    });
    
}


