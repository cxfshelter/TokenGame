//
//  TraingHeroUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MainScene.h"
#include "TraingHeroUI.h"
#include "AchievementSystem.h"

TraingHeroUI* TraingHeroUI::sm_TraingHeroUI = nullptr;


TraingHeroUI* TraingHeroUI::getInstance()
{
    if (sm_TraingHeroUI == nullptr)
    {
        sm_TraingHeroUI = new (std::nothrow) TraingHeroUI();
        if (sm_TraingHeroUI->Init()) {}
    }
    return sm_TraingHeroUI;
}

bool TraingHeroUI::Init()
{

    return true;
}

void TraingHeroUI::UnLoad()
{
    
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    UIBase::UnLoad();
}
void TraingHeroUI::UpdateUI()
{
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Item0 = layout_Template->getChildByName<Layout*>("Layout_Item0");
    
    auto layout_Item1 = layout_Template->getChildByName<Layout*>("Layout_Item1");

    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto lView_List = layout_Center->getChildByName<ListView*>("LView_List");
        //每次清空 子控件
    lView_List->removeAllChildren();
    
    //招募
    auto layout_Item = layout_Item0->clone();
    lView_List->addChild(layout_Item);
    
    auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
    btn_Name->addClickEventListener([this](Ref* sender)
        {
            RecruitDialog();
        });

    vector<pair<int, Value>> viv_LiveHero;
    
    ValueMap* vm_LiveHero =  &(UserData::getInstance()->GetUserData(UserDataEnum::LiveHero)->asValueMap());
    
    UserData::getInstance()->VMSKToVIV_Sort(vm_LiveHero, &viv_LiveHero);
    
    for (auto iter = viv_LiveHero.begin(); iter != viv_LiveHero.end(); iter++)
    {
        auto layout_Item = layout_Item1->clone();
        lView_List->addChild(layout_Item);
        
        auto cfgId = iter->first;
        
        CfgDataRow dataRow(cfgId);
        
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        auto btn_Name = layout_Item->getChildByName<Button*>("Btn_Name");
        btn_Name->setTitleText(langText.c_str());
        
        auto btn_Num = layout_Item->getChildByName<Button*>("Btn_Num");
        btn_Num->setTitleText(StringUtils::format("%d", iter->second.asInt()));
        
        btn_Name->addClickEventListener([=](Ref* sender)
        {
            CfgDataRow dataRow(cfgId);
            auto vv_Hero = &dataRow.GetValue(CfgField::UpgradeID_V)->asValueVector();
            if (vv_Hero->empty())
            {
                auto langId = (int)LanguageIdEnum::HaveMaxLevelHeroId;
                auto langText = MultiLanguage::getInstance()->GetText(langId);
                MoveText::getInstance()->AddDefaultTextData(m_RootWidget, langText);
                
            }else
            {
                UpgradeUI(cfgId);
            }
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
        
        auto btn_Explain = layout_Item->getChildByName<Button*>("Btn_Explain");
        btn_Explain->addClickEventListener([this, cfgId](Ref* sender)
        {
           CfgDataRow dataRow(cfgId);
            auto langId = dataRow.GetValue(CfgField::Details)->asInt();
           auto langText = MultiLanguage::getInstance()->GetText(langId);
           TextExplainNode::getInstance()->UpdateText(m_RootWidget, &langText, cfgId);
        });
        
        UIUtils::getInstance()->showProperImg(btn_Name, cfgId);
        
        auto text_Skill = layout_Item->getChildByName<Text*>("Text_Skill");
        auto vi_skill = dataRow.GetValue(CfgField::SkillID_V)->asValueVector();
        auto iter_skill = vi_skill.begin(); auto skillId = iter_skill->asInt();
        CfgDataRow dataRow_Skill(skillId);
        
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SkillId);
        langText += MultiLanguage::getInstance()->GetText(dataRow_Skill.GetValue(CfgField::Name)->asInt());
        text_Skill->setString(langText);
        
        auto text_Power = layout_Item->getChildByName<Text*>("Text_Power");
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::PowerId);
        langText += StringUtils::format("%d", dataRow.GetValue(CfgField::Atk)->asInt());
        text_Power->setString(langText);
        
        auto text_Life = layout_Item->getChildByName<Text*>("Text_Life");
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::LifeId);
        langText += StringUtils::format("%d", dataRow.GetValue(CfgField::Hp)->asInt());
        text_Life->setString(langText);
        
        auto text_Speed = layout_Item->getChildByName<Text*>("Text_Speed");
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SpeedId);
        langText  += StringUtils::format("%g", dataRow.GetValue(CfgField::Spd_F)->asFloat());
        text_Speed->setString(langText);

    }
    lView_List->refreshView();
}

//招募 英雄
void TraingHeroUI::RecruitDialog()
{
    auto layout_Recruit = m_RootWidget->getChildByName<Layout*>("Layout_Recruit");
    layout_Recruit->setVisible(true);
    
    auto langId = (int)LanguageIdEnum::Recruit_RiskerId;
    auto langText = MultiLanguage::getInstance()->GetText( langId);
    auto text_Title = layout_Recruit->getChildByName<Text*>("Text_Title_Recruit");
    text_Title->setString(langText);
    
    for (auto i = 1; i <= 3; i++)
    {
        auto btn_Name = StringUtils::format("Btn_Hero%d", i);
        auto btn_Hero = layout_Recruit->getChildByName<Button*>(btn_Name);
        
        auto heroId = 0;
        auto heroLangId = 0;
        switch (i)
        {
            case 1:
                heroId = (int)CfgIdEnum::RecruitStrongManId;
                heroLangId = (int)LanguageIdEnum::StrongManEvolutionId;
                break;
            case 2:
                heroId = (int)CfgIdEnum::RecruitPsykerId;
                heroLangId = (int)LanguageIdEnum::PsykerEvolutionId;
                break;
            case 3:
                heroId = (int)CfgIdEnum::RecruitMechaId;
                heroLangId = (int)LanguageIdEnum::MechaEvolutionId;
                break;
        }
        
        CfgDataRow heroDataRow(heroId);
        langId = heroDataRow.GetValue(CfgField::Name)->asInt();
        langText = MultiLanguage::getInstance()->GetText(langId);
        btn_Hero->setTitleText(langText);
        
        auto text_AttirbuteName = StringUtils::format("Text_Attribute%d", i);
        auto text_Attribute = btn_Hero->getChildByName<Text*>(text_AttirbuteName);
        
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::LifeId);
        langText += StringUtils::format("%d", heroDataRow.GetValue(CfgField::Hp)->asInt());
        
        langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::PowerId);
        langText += StringUtils::format("%d", heroDataRow.GetValue(CfgField::Atk)->asInt());
        
        langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SpeedId);
        langText += StringUtils::format("%g", heroDataRow.GetValue(CfgField::Spd_F)->asFloat());
        
        text_Attribute->setString(langText);

        
        auto vmi = heroDataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
        string text = "";
        for (auto iter = vmi.begin(); iter != vmi.end(); iter++)
        {
            CfgDataRow dr(iter->first);
            langId = dr.GetValue(CfgField::Name)->asInt();
            langText = MultiLanguage::getInstance()->GetText(langId);
            text += StringUtils::format("%s x%d ", langText.c_str(), iter->second.asInt());
        }
        
        auto text_HeroInfoName = StringUtils::format("Text_HeroInfo%d", i);
        auto text_HeroInfo =  btn_Hero->getChildByName<Text*>(text_HeroInfoName);
        langText = MultiLanguage::getInstance()->GetText(heroLangId);
        text_HeroInfo->setString(text + langText);
        
        btn_Hero->addClickEventListener([=](Ref* sender)
         {
             layout_Recruit->setVisible(false);
             ConfirmUI(0, heroId);
         });
        
    }
    
    auto btn_Cancel = layout_Recruit->getChildByName<Button*>("Btn_Cancel_Recruit");
    
    langId = (int)LanguageIdEnum::CancelLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Cancel->setTitleText(langText);
    
    btn_Cancel->addClickEventListener([=](Ref* sender)
     {
         layout_Recruit->setVisible(false);
     });
    
    
}

void TraingHeroUI::ConfirmUI(int oldCfgId, int newCfgId)
{
    auto layout_Confirm = m_RootWidget->getChildByName<Layout*>("Layout_Confirm");
    layout_Confirm->setVisible(true);
    
    auto btn_Confirm = layout_Confirm->getChildByName<Button*>("Btn_Confirm");
    auto langId = (int)LanguageIdEnum::ConfirmLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    
    btn_Confirm->setTitleText(langText);
    btn_Confirm->addClickEventListener([this, oldCfgId, newCfgId, layout_Confirm](Ref* sender)
       {
           vector<pair<int, int> > lackData;
           
           CfgDataRow dataRow(newCfgId);
           auto vmi = dataRow.GetValue(CfgField::MaterialID_M);
           bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vmi->asIntMultiKeyMap() , &lackData);
           
           // best test
//           delResutl = true;
           
           if (delResutl)
           {
               auto val_LiveHero = UserData::getInstance()->GetUserData(UserDataEnum::LiveHero);
               UserData::getInstance()->Update_VMI_Fun(&val_LiveHero->asValueMap(), newCfgId, 1);
               if (oldCfgId > 0)
               {
                   UserData::getInstance()->Update_VMI_Fun(&val_LiveHero->asValueMap(), oldCfgId, -1);
               }
               UpdateUI();
               UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, newCfgId);
               AchievementSystem::getInstance()->CheckAchievement(UserEventType::TrainHero, newCfgId);
               MsgListNode::getInstance()->NewMsg(newCfgId, CfgField::Details);
               layout_Confirm->setVisible(false);
               
           }else
           {
               // 弹出引导钻石消费界面
               int totalNum = UIUtils::getInstance()->getLackCostDiamond(&lackData);
               string showText = StringUtils::format(MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::Diamond_ToGet).c_str(), totalNum) + MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Name)->asInt());
               UIUtils::getInstance()->addLeadBuyLayer(this, showText, totalNum,
                                                       [=]()
                                                       {
                                                           UIUtils::getInstance()->costUpgNeededItem(&vmi->asIntMultiKeyMap(), &lackData);
                                                           auto val_LiveHero = UserData::getInstance()->GetUserData(UserDataEnum::LiveHero);
                                                           UserData::getInstance()->Update_VMI_Fun(&val_LiveHero->asValueMap(), newCfgId, 1);
                                                           if (oldCfgId > 0)
                                                           {
                                                               UserData::getInstance()->Update_VMI_Fun(&val_LiveHero->asValueMap(), oldCfgId, -1);
                                                           }
                                                           UpdateUI();
                                                           UserEventContainer::getInstance()->Send(UserEventType::GetItemEvent, newCfgId);
                                                           AchievementSystem::getInstance()->CheckAchievement(UserEventType::TrainHero, newCfgId);
                                                           MsgListNode::getInstance()->NewMsg(newCfgId, CfgField::Details);
                                                           layout_Confirm->setVisible(false);
                                                       }
                                                       );
               
               MoveText::getInstance()->LackResText(m_RootWidget, &lackData);
           }
           
       });
    
    auto btn_Cancel = layout_Confirm->getChildByName<Button*>("Btn_Cancel");
    langId = (int)LanguageIdEnum::CancelLangId;
    langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Cancel->setTitleText(langText);
    btn_Cancel->addClickEventListener([this, layout_Confirm](Ref* sender)
      {
          layout_Confirm->setVisible(false);
      });
    

    CfgDataRow dataRow(newCfgId);
    auto text_Title = layout_Confirm->getChildByName<Text*>("Text_Title");

    langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::TrainingId);
    auto heroName = MultiLanguage::getInstance()->GetText(dataRow.GetValue(CfgField::Name)->asInt());
    text_Title->setString(StringUtils::format(langText.c_str(), heroName.c_str()));

    
    auto text_Skill = layout_Confirm->getChildByName<Text*>("Text_Skill");
    auto vi_skill = dataRow.GetValue(CfgField::SkillID_V)->asValueVector();
    auto iter_skill = vi_skill.begin(); auto skillId = iter_skill->asInt();
    
    CfgDataRow dataRow_Skill(skillId);
    
    langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SkillId);
    langText += MultiLanguage::getInstance()->GetText(dataRow_Skill.GetValue(CfgField::Name)->asInt());
    text_Skill->setString(langText);
    
    auto text_Buffer = layout_Confirm->getChildByName<Text*>("Text_Buffer");
    auto bufferData = dataRow_Skill.GetValue(CfgField::BuffID_V)->asValueVector();
    if (!bufferData.empty())
    {
        auto iter_buff = bufferData.begin(); auto bufferId = iter_buff->asInt();
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::BufferId);
        CfgDataRow dataRow_Buffer(bufferId);
        langText += MultiLanguage::getInstance()->GetText(dataRow_Buffer.GetValue(CfgField::Name)->asInt());
        text_Buffer->setString(langText); text_Buffer->setVisible(true);
    }else
    {
        text_Buffer->setVisible(false);
    }
    
    
    auto text_Attribute = layout_Confirm->getChildByName<Text*>("Text_Attribute");
    
    langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::LifeId);
    langText += StringUtils::format("%d", dataRow.GetValue(CfgField::Hp)->asInt());
    
    langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::PowerId);
    langText += StringUtils::format("%d", dataRow.GetValue(CfgField::Atk)->asInt());
    
    langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SpeedId);
    langText += StringUtils::format("%g", dataRow.GetValue(CfgField::Spd_F)->asFloat());
    text_Attribute->setString(langText);
    
    
}


void TraingHeroUI::UpgradeUI(int cfgId)
{
    auto layout_Upgrade = m_RootWidget->getChildByName<Layout*>("Layout_Upgrade");
    layout_Upgrade->setVisible(true);

    auto text_Title = layout_Upgrade->getChildByName<Text*>("Text_Title");
    text_Title->setString(MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::OptionHeroId));
    
    auto lView_List = layout_Upgrade->getChildByName<ListView*>("LView_List");
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Name = layout_Template->getChildByName<Layout*>("Layout_Upgrade_Name");
    
    auto layout_Cancel = layout_Template->getChildByName<Layout*>("Layout_Upgrade_Cancel");

    CfgDataRow dataRow(cfgId);
    auto vv_Hero = &dataRow.GetValue(CfgField::UpgradeID_V)->asValueVector();
        
    lView_List->removeAllChildren();
    
    for (auto iter = vv_Hero->begin(); iter != vv_Hero->end(); iter++)
    {
        auto layout_NameItem = layout_Name->clone();
        lView_List->addChild(layout_NameItem);
        
        auto heroId = iter->asInt();
        CfgDataRow dataRow(heroId);
        
        auto btn_name = layout_NameItem->getChildByName<Button*>("Btn_Name");
        auto langId = dataRow.GetValue(CfgField::Name)->asInt();
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        
        btn_name->setTitleText(langText.c_str());
        btn_name->addClickEventListener([=](Ref* sender)
        {
            ConfirmUI(cfgId, heroId);
            layout_Upgrade->setVisible(false);
        });
        
        auto vmi = dataRow.GetValue(CfgField::MaterialID_M)->asIntMultiKeyMap();
        
        string text;
        string text2;
        auto text_items = layout_NameItem->getChildByName<Text*>("Text_Items1");
        auto text_items2 = layout_NameItem->getChildByName<Text*>("Text_Items2");
        for (auto iter = vmi.begin(); iter != vmi.end(); iter++)
        {
            int cfgId = iter->first;
            CfgDataRow dr(cfgId);
            langId = dr.GetValue(CfgField::Name)->asInt();
            langText = MultiLanguage::getInstance()->GetText(langId);
            
            if (text.size() <= 0) {
                if(!UIUtils::getInstance()->isStuffEnough(cfgId, iter->second.asInt()))
                {
                    text_items->setColor(Color3B(128, 128, 128));
                }
                text = StringUtils::format("%s x%d ", langText.c_str(), iter->second.asInt());
            }
            else {
                if(!UIUtils::getInstance()->isStuffEnough(cfgId, iter->second.asInt()))
                {
                    text_items2->setColor(Color3B(128, 128, 128));
                }
                text2 = StringUtils::format("%s x%d ", langText.c_str(), iter->second.asInt());
            }
        }
        text_items->setString(text);
        text_items2->setString(text2);
        
        UIUtils::getInstance()->showProperImg(btn_name, cfgId);
        
    }
    
    auto layout_CancelItem = layout_Cancel->clone();
    
    lView_List->addChild(layout_CancelItem);
    
    auto btn_Cancel = layout_CancelItem->getChildByName<Button*>("Btn_Cancel");
    
    auto langId = (int)LanguageIdEnum::CancelLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    btn_Cancel->setTitleText(langText);

    btn_Cancel->addClickEventListener([=](Ref* sender)
      {
          layout_Upgrade->setVisible(false);
      });
    
    lView_List->refreshView();
}

void TraingHeroUI::LoadFinish()
{
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    
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


