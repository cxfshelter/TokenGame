//
//  MoveText.cpp
//  TokenGame
//
//  Created by token on 15/4/10.
//
//

#include "Common.h"
#include "MoveText.h"


MoveText* MoveText::sm_MoveText = nullptr;

MoveText* MoveText::getInstance()
{
    if (sm_MoveText == nullptr)
    {
        sm_MoveText = new (std::nothrow) MoveText();
        if (sm_MoveText->Init()) {}
    }
    return sm_MoveText;
}

bool MoveText::Init()
{
    return true;
}

void MoveText::ClearTextData()
{
    m_LastUpTextWidget = nullptr;
    m_LastDownTextWidget = nullptr;
}

//缺少资源 移动信息
void MoveText::LackResText(Node* node, vector<pair<int, int>>* lackData)
{
    MoveText::getInstance()->AddDefaultTextData(node, getLackText(lackData));
}

string MoveText::getLackText(vector<pair<int, int>>* lackData)
{
    string result = "";
    auto itemId = 0; auto itemNum = 0;
    for (auto iter = lackData->begin(); iter != lackData->end(); iter++)
    {
        
        itemId = iter->first;
        itemNum = iter->second;
        auto langId = (int)LanguageIdEnum::LackResLangId;
        auto langText = MultiLanguage::getInstance()->GetText(langId);
        
        langId = CfgData::getInstance()->GetValue(itemId, CfgField::Name)->asInt();
        langText += MultiLanguage::getInstance()->GetText(langId);
        langText = StringUtils::format("%s x%d \n", langText.c_str(), itemNum);
        
        result += langText;
    }
    return result;
}

//获取了资源 移动信息
void MoveText::GetResText(Node* node, int itemId, int itemNum)
{
    
    auto langId = (int)LanguageIdEnum::GetResLangId;
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    
    langId = CfgData::getInstance()->GetValue(itemId, CfgField::Name)->asInt();
    
    langText += MultiLanguage::getInstance()->GetText(langId);
    
    langText = StringUtils::format("%s x%d ", langText.c_str(), itemNum);
    MoveText::getInstance()->AddDefaultTextData(node, langText);
    
    
}

void MoveText::AddDefaultTextData(Node* node, string text)
{
    MoveText::TextData textData;
    textData.FontSize = 24;
    textData.TextInfo = text;
    textData.Color = Color3B::WHITE;
    textData.ParenNode = node;
    textData.Direction = MoveText::Direction::Down;
    auto size = Director::getInstance()->getVisibleSize() /2;
    textData.PositionX = size.width;textData.PositionY = size.height;
    textData.isBattleText = false;
    AddTextData(textData);
}

void MoveText::AddTextData(TextData textData)
{
    
    float new_Axis = 0.0f;
    float old_Axis = 0.0f;
    
    auto textWidget = Text::create(textData.TextInfo, m_TTFFontName, textData.FontSize);
    
    switch (textData.Direction) {
        case Direction::Up :
            if (m_LastUpTextWidget != nullptr)
            {
                new_Axis = textData.PositionY;
                old_Axis = m_LastUpTextWidget->getPositionY();
                if (old_Axis < new_Axis + m_LastUpTextWidget->getContentSize().height)
                {
                    textData.PositionY =  old_Axis - m_LastUpTextWidget->getContentSize().height;
                    if(textData.PositionX < 100)  textData.PositionX = 120;
                }
            }
            m_LastUpTextWidget = textWidget;
            break;
        case Direction::Down :
            
            if (m_LastDownTextWidget != nullptr)
            {
                new_Axis = textData.PositionY;
                old_Axis = m_LastDownTextWidget->getPositionY();
                if (old_Axis > new_Axis - m_LastDownTextWidget->getContentSize().height)// * 0.8
                {
                    textData.PositionY =  old_Axis + m_LastDownTextWidget->getContentSize().height;// * 0.8;
                    if (textData.PositionX > Director::getInstance()->getVisibleSize().height)
                    {
                        textData.PositionX = Director::getInstance()->getVisibleSize().height - 120;
                    }
                }
            }
            m_LastDownTextWidget = textWidget;
            break;
    }
    
    
//    textWidget->setScale(0.1F);
    textWidget->setColor(textData.Color);
    textWidget->setTextAreaSize(Size(450,40));
    textWidget->setTextHorizontalAlignment(TextHAlignment::CENTER);
    
    textData.TextWidget = textWidget;
    textData.ParenNode->addChild(textWidget);
    
    textWidget->setPositionX(textData.PositionX);
    textWidget->setPositionY(textData.PositionY);
    
    Vec2 movePos;
    switch (textData.Direction)
    {
        case Direction::Up :
            if (textData.isBattleText) {
                movePos = Vec2(textWidget->getPositionX(), textWidget->getPositionY() + 70);
            }
            else {
                movePos = Vec2(textWidget->getPositionX(), textWidget->getPositionY() + 200);
            }
            break;
        case Direction::Down :
            if (textData.isBattleText) {
                movePos = Vec2(textWidget->getPositionX(), textWidget->getPositionY() - 70);
            }
            else {
                movePos = Vec2(textWidget->getPositionX(), textWidget->getPositionY() - 200);
            }
            break;
    }
    //同时执行移动和渐出
//    auto spawn = Spawn::create(moveTo, fadeOut, nullptr);
//    顺序执行多个动作
//    auto seq= Sequence::create(scaleBig,scaleLitt,spawn,
    
    // best modify
    //渐出
    auto fadeOut = FadeOut::create(0.8f);
    //放大
    auto scaleBig = ScaleTo::create(0.2F, 2.5F);
    //缩小
    auto scaleLitt = ScaleTo::create(0.2F, 1.50F);
    //移动
    auto moveTo = MoveTo::create(1.5F, movePos);
    //结束回调
    auto callFunc = CallFunc::create([this, textData, textWidget]()
                                     {
                                         if (textWidget->getParent())
                                         {
                                             textWidget->stopAllActions();
                                             textWidget->removeFromParent();
                                         }
                                         
                                         switch (textData.Direction)
                                         {
                                             case Direction::Up:
                                                 if(m_LastUpTextWidget == textData.TextWidget)
                                                 {
                                                     m_LastUpTextWidget = nullptr;
                                                 }
                                                 break;
                                             case Direction::Down:
                                                 if(m_LastDownTextWidget == textData.TextWidget)
                                                 {
                                                     m_LastDownTextWidget = nullptr;
                                                 }
                                                 
                                                 break;
                                         }
                                         
                                     });
    
    if (textData.isBattleText) {
        // 战斗文字
        textWidget->setScale(1.3F);
        moveTo = MoveTo::create(0.4F, movePos);
        
        auto seq= Sequence::create(moveTo, callFunc, nullptr);
        auto seq2 = Sequence::create(DelayTime::create(1), fadeOut, nullptr);
        
        textWidget->runAction(seq);
        textWidget->runAction(seq2);
    }
    else {
        // 非战斗
        textWidget->setScale(0.1F);
        auto seq= Sequence::create(scaleBig, scaleLitt, moveTo, callFunc, nullptr);
        textWidget->runAction(seq);
    }
    
 }



