//
//  TextExplainNode.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "TextExplainNode.h"

TextExplainNode* TextExplainNode::sm_TextExplainNode = nullptr;

TextExplainNode* TextExplainNode::getInstance()
{
    if (sm_TextExplainNode == nullptr)
    {
        sm_TextExplainNode = new (std::nothrow) TextExplainNode();
        if(sm_TextExplainNode->Init() == false)
        {
            
        }
        
    }
    return sm_TextExplainNode;

}

bool TextExplainNode::Init()
{
    return true;
}

void TextExplainNode::UpdateText(Node* rootWidget, const string* text, int cfgId)
{
    
    auto node_TextExplain = rootWidget->getChildByName<Node*>("Node_TextExplain");
    node_TextExplain->setVisible(true);
    
    auto layout_FullScreen = node_TextExplain->getChildByName<Layout*>("Layout_FullScreen");

    layout_FullScreen->setVisible(true);
    layout_FullScreen->setTouchEnabled(true);
    layout_FullScreen->setSwallowTouches(false);
    layout_FullScreen->addClickEventListener([=](Ref* sender)
     {
         node_TextExplain->setVisible(false);
         layout_FullScreen->setVisible(false);
         layout_FullScreen->setTouchEnabled(false);
     });

    auto layout_Background = node_TextExplain->getChildByName<Layout*>("Layout_Background");
    auto text_Explain = layout_Background->getChildByName<Text*>("Text_Explain");
    text_Explain->setString(text->c_str());
    
    auto layout_restrain = layout_Background->getChildByName<Layout*>("Layout_Restrain");
    if ( cfgId == 0 || (cfgId / 1000000 != 205) ) {
        layout_restrain->setVisible(false);
        return;
    }
    layout_restrain->setVisible(true);
    
    auto layout_propMain = layout_restrain->getChildByName<Layout*>("Layout_PropMain");
    UIUtils::getInstance()->showProperImg(layout_propMain, cfgId, false);
    
    CfgDataRow dataRow(cfgId);
    // 属性配置表的id与item的属性字段id不统一，故作如下转换
    auto properId = (int)ProCfgId::CfgBaseId + dataRow.GetValue(CfgField::PropertyID)->asInt();
    CfgDataRow proDataRow(properId);
    
    // 1001 ~ 1005
    float result = 1.0;
    vector<int> vLeft(2, 0);
    vector<int> vRight(2, 0);
    int leftIdx = 0;
    int rightIdx = 0;
    for (int proId = 1001; proId <= 1005; ++proId) {
        switch (proId) {
            case (int)ProCfgId::MeleeCo :
                result = proDataRow.GetValue(CfgField::MeleeCo_F)->asFloat();
                break;
            case (int)ProCfgId::FighterCo :
                result = proDataRow.GetValue(CfgField::FighterCo_F)->asFloat();
                break;
            case (int)ProCfgId::MagicCo :
                result = proDataRow.GetValue(CfgField::MagicCo_F)->asFloat();
                break;
            case (int)ProCfgId::RangedCo :
                result = proDataRow.GetValue(CfgField::RangedCo_F)->asFloat();
                break;
            case (int)ProCfgId::FlyingCo :
                result = proDataRow.GetValue(CfgField::FlyingCo_F)->asFloat();
                break;
        }
        
        if (result < 1) {
            vLeft[leftIdx++] = proId;
        }
        else if(result > 1) {
            vRight[rightIdx++] = proId;
        }
    }
    
    int i = 1;
    for(int proId : vLeft) {
        auto leftProNode = layout_restrain->getChildByName(StringUtils::format("Layout_PropLeft%d", i++));
        UIUtils::getInstance()->showRealProper(leftProNode, proId, false);
    }
    
    i = 1;
    for(int proId : vRight) {
        auto RightProNode = layout_restrain->getChildByName(StringUtils::format("Layout_PropRight%d", i++));
        UIUtils::getInstance()->showRealProper(RightProNode, proId, false);
    }
    
    // 技能、buff、属性描述
    auto text_Buff = layout_restrain->getChildByName<Text *>("Text_Buff");
    auto text_Skill = layout_restrain->getChildByName<Text *>("Text_Skill");
    auto text_Attr = layout_restrain->getChildByName<Text *>("Text_Attr");
    
    auto vi_skill = dataRow.GetValue(CfgField::SkillID_V)->asValueVector();
    auto iter_skill = vi_skill.begin();
    auto skillId = iter_skill->asInt();
    
    CfgDataRow dataRow_Skill(skillId);
    
    auto langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SkillId);
    langText += MultiLanguage::getInstance()->GetText(dataRow_Skill.GetValue(CfgField::Name)->asInt());
    text_Skill->setString(langText);
    
    auto bufferData = dataRow_Skill.GetValue(CfgField::BuffID_V)->asValueVector();
    if (!bufferData.empty()) {
        auto iter_buff = bufferData.begin();
        auto bufferId = iter_buff->asInt();
        langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::BufferId);
        CfgDataRow dataRow_Buffer(bufferId);
        langText += MultiLanguage::getInstance()->GetText(dataRow_Buffer.GetValue(CfgField::Name)->asInt());
        text_Buff->setString(langText);
        text_Buff->setVisible(true);
    }else
    {
        text_Buff->setVisible(false);
    }
    
    langText = MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::LifeId);
    langText += StringUtils::format("%d", dataRow.GetValue(CfgField::Hp)->asInt());
    
    langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::PowerId);
    langText += StringUtils::format("%d", dataRow.GetValue(CfgField::Atk)->asInt());
    
    langText += " " + MultiLanguage::getInstance()->GetText((int)LanguageIdEnum::SpeedId);
    langText += StringUtils::format("%g", dataRow.GetValue(CfgField::Spd_F)->asFloat());
    
    text_Attr->setString(langText);
    
}