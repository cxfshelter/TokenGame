//
//  AchievementUI.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//
#include "BattlePubScene.h"
#include "AchievementUI.h"
#include "AchievementSystem.h"

AchievementUI* AchievementUI::sm_AchievementUI = nullptr;


AchievementUI* AchievementUI::getInstance()
{
    if (sm_AchievementUI == nullptr)
    {
        sm_AchievementUI = new (std::nothrow) AchievementUI();
        if (sm_AchievementUI->Init()) {}
    }
    return sm_AchievementUI;
}

bool AchievementUI::Init()
{
    return true;
}

void AchievementUI::UnLoad()
{
    UIBase::UnLoad();
    MsgListNode::getInstance()->UnLoadMsg();
}

void AchievementUI::UpdateUI()
{
    auto layout_Center = m_RootWidget->getChildByName<Layout*>("Layout_Center");
    
    auto text_Desc = layout_Center->getChildByName<Text*>("Text_Desc");
    
    auto langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::NextAchievementVal);
    
    int nextAchievementNum = 0;
    
    auto achievementTalent = CfgData::getInstance()->GetCfgTableId("AchievementTalent");
    int nowAchievementNum = UserData::getInstance()->GetUserData(UserDataEnum::AchievementNum)->asInt();
    
    for (auto iter = achievementTalent->begin(); iter != achievementTalent->end(); iter++)
    {
        CfgDataRow dataRow(*iter);
        nextAchievementNum = dataRow.GetValue(CfgField::Parameter)->asInt();
        if ( nextAchievementNum > nowAchievementNum) break;
    }
    if (nextAchievementNum > nowAchievementNum)
    {
        langText = StringUtils::format(langText.c_str(), nextAchievementNum);
        text_Desc->setString(langText);
    }else
    {
        text_Desc->setVisible(false);
    }
    
    
    auto Text_Num = layout_Center->getChildByName<Text*>("Text_Num");
    
    langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::NowAchievementVal);
    
    Text_Num->setString(StringUtils::format("%s%d", langText.c_str(), nowAchievementNum));
    
    
    auto lview_List = layout_Center->getChildByName<ListView*>("LView_List");
    
    auto layout_Template = m_RootWidget->getChildByName<Layout*>("Layout_Template");
    
    auto layout_Template_Item = layout_Template->getChildByName<Layout*>("Layout_Item");
    
    int cfgId = 0,level = 0, langId = 0;
    auto tableIdList = CfgData::getInstance()->GetCfgTableId("Achievement");
    for (auto iter = tableIdList->begin(); iter != tableIdList->end();)
    {
        auto layout_Item = layout_Template_Item->clone();
        lview_List->addChild(layout_Item);
        
        for (auto i = 0; i < 3 ; i++ ,iter++)
        {
            if (iter == tableIdList->end())
            {
                // 隐藏多余的格子
                auto startUnVisible = layout_Item->getChildByName<Node *>(StringUtils::format("Start%d", i));
                auto btnUnVisible = layout_Item->getChildByName<Node *>(StringUtils::format("Btn_Name%d", i));
                auto spriteUnVisible = layout_Item->getChildByName<Node *>(StringUtils::format("Sprite_Finish%d", i));
                startUnVisible->setVisible(false);
                btnUnVisible->setVisible(false);
                spriteUnVisible->setVisible(false);
                break;
            }
            
            cfgId = *iter;
            auto btn_Name = layout_Item->getChildByName<Button*>(StringUtils::format("Btn_Name%d", i));
            btn_Name->setVisible(true);
            
            CfgDataRow dataRow(cfgId);
            langId = dataRow.GetValue(CfgField::Name)->asInt();
            langText = MultiLanguage::getInstance()->GetText(langId);
            btn_Name->setTitleText(langText);
            
            auto finish_Data = &UserData::getInstance()->GetUserData(UserDataEnum::FinishAchievement)->asValueVector();
            bool isFinish = find(finish_Data->begin(), finish_Data->end(), Value(cfgId)) != finish_Data->end();
            
            btn_Name->addClickEventListener([=](Ref* sender)
                {
                    CfgDataRow dataRow(cfgId);
                    auto langId = dataRow.GetValue(CfgField::Details)->asInt();
                    auto langText = MultiLanguage::getInstance()->GetText(langId);
                    auto rewardText = StringUtils::format("\n奖励点数：%d", dataRow.GetValue(CfgField::Reward)->asInt());
                    string progressText = "\n当前进度：已完成";
                    if (!isFinish)
                    {
                        progressText = StringUtils::format("\n当前进度：%d", AchievementSystem::getInstance()->getCurProgressNum(cfgId));
                    }
                    
                    auto str = langText + rewardText + progressText;
                    TextExplainNode::getInstance()->UpdateText(m_RootWidget, &str);
                });
            
            
            if (isFinish)
            {
                auto sprite_Finish = layout_Item->getChildByName<Button*>(StringUtils::format("Sprite_Finish%d", i));
                sprite_Finish->setVisible(true);
            }
            
            level = dataRow.GetValue(CfgField::Level)->asInt();
            auto startName = StringUtils::format("Start%d", i);
            auto layout_Start = layout_Item->getChildByName<Layout*>(startName);
            if (layout_Start != nullptr)
            {
                for (auto i = 1; i <= level; i++)
                {
                    auto startName2 = StringUtils::format("%s_%d", startName.c_str(), i - 1);
                    auto imgStart = layout_Start->getChildByName<ImageView*>(startName2);
                    if (imgStart != nullptr) imgStart->setVisible(true);
                }
            }
        }
    }
}

void AchievementUI::LoadFinish()
{
    MsgListNode::getInstance()->LoadMsg(m_RootWidget);
    
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    
    auto btn_Left = layout_title->getChildByName<Button*>("Btn_Left");
    
    btn_Left->addClickEventListener([this](Ref* sender)
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
                                          },
                                          nullptr
                                          );
}


