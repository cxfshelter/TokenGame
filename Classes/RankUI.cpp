//
//  RankUI.cpp
//  TokenGame
//
//  Created by best on 15/5/11.
//
//

#include "RankUI.h"
#include "Network.h"
#include "BattlePubScene.h"

RankUI* RankUI::sm_RankUI = nullptr;

RankUI* RankUI::getInstance()
{
    if (sm_RankUI == nullptr) {
        sm_RankUI = new (std::nothrow) RankUI();
        if (sm_RankUI->Init()) { }
    }
    return sm_RankUI;
}

bool RankUI::Init()
{
    return true;
}

void RankUI::UnLoad()
{
    MsgListNode::getInstance()->UnLoadMsg();
    MoveText::getInstance()->ClearTextData();
    UIBase::UnLoad();
}

void RankUI::UpdateUI()
{
    checkName();
}

void RankUI::LoadFinish()
{
    auto layout_Btns = m_RootWidget->getChildByName<Layout*>("Layout_Btns");
    auto btn_Close = layout_Btns->getChildByName<Button*>("Button_Close");
    btn_Close->addClickEventListener([this](Ref* sender)
                                    {
                                        this->UnLoad();
                                        auto scene = BattlePubScene::createScene();
                                        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                    });
    
    auto btn_rank = layout_Btns->getChildByName<Button*>("Button_Switch");
    btn_rank->addClickEventListener([this](Ref* sender)
                                    {
                                        listShowSwitch();
                                    });
    
    auto layout_title = m_RootWidget->getChildByName<Layout*>("Layout_Title");
    auto btn_left = layout_title->getChildByName<Layout*>("Btn_Left");
    btn_left->addClickEventListener([this](Ref* sender)
                                     {
                                         this->UnLoad();
                                         auto scene = BattlePubScene::createScene();
                                         Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
                                     });
    
}

void RankUI::listShowSwitch()
{
    auto layout_Template_Item = m_RootWidget->getChildByName<Layout*>("Layout_template");
    auto layout_ListTitles = m_RootWidget->getChildByName<Layout*>("Layout_ListTitle");
    
    auto lview_list = layout_ListTitles->getChildByName<ListView*>("LView_RankList");
    
    auto layout_Btns = m_RootWidget->getChildByName<Layout*>("Layout_Btns");
    auto btn_switch = layout_Btns->getChildByName<Button*>("Button_Switch");
    
    auto text_switch = layout_ListTitles->getChildByName<Text*>("Text_switch");
    
    Network::getInstance()->GetRanking(
                                       [=](vector<tuple<int,int,string>>* explore,
                                           vector<tuple<int,int,string>>* arena)
                                       {
                                           if (!m_RootWidget) {
                                               // 界面关闭，返回
                                               return;
                                           }
                                           
                                           vector<tuple<int,int,string>>* vecs;
                                           if (m_isArena)
                                           {
                                               vecs = arena;
                                               btn_switch->setTitleText("探索榜");
                                               text_switch->setString("虚空竞技场");
                                           }
                                           else
                                           {
                                               vecs = explore;
                                               btn_switch->setTitleText("竞技榜");
                                               text_switch->setString("探索度");
                                           }
                                           if(vecs == nullptr)
                                           {
                                               return;
                                           }
                                           
                                           lview_list->removeAllChildren();
                                           for(auto exploreItem : *vecs)
                                           {
                                               int rankNum = std::get<0>(exploreItem);
                                               int rankScore = std::get<1>(exploreItem);
                                               string rankName = std::get<2>(exploreItem);
                                               
                                               auto layout_Item = layout_Template_Item->clone();
                                               lview_list->addChild(layout_Item);
                                               Text* itemNum = layout_Item->getChildByName<Text*>("Text_RankNum");
                                               Text* itemName = layout_Item->getChildByName<Text*>("Text_RankName");
                                               Text* itemScore = layout_Item->getChildByName<Text*>("Text_RankDegree");
                                               itemNum->setString(StringUtils::format("%d", rankNum));
                                               itemName->setString(rankName);
                                               itemScore->setString(StringUtils::format("%d", rankScore));
                                           }
                                           m_isArena = !m_isArena;
                                       }
                                       );
}

void RankUI::checkName()
{
    // 根据imei码，服务器返回有无该名字，有则不能再输入
    auto userName = UserData::getInstance()->GetUserData(UserDataEnum::UserName)->asString();
//    if (userName.compare("") == 0) {
    if(true) {
        // 该存档没有输入用户名
        inputName();
    }
    else {
        listShowSwitch();
    }
    
}

void RankUI::inputName()
{
    auto layout_intputName = m_RootWidget->getChildByName<Layout*>("Layout_InputName");
    if (layout_intputName) layout_intputName->setVisible(true);
    
    auto textField_inputName = layout_intputName->getChildByName<TextFieldTTF *>("TextField_InputName");
    textField_inputName->setDelegate(getInstance());
    
    auto btn_confirm = layout_intputName->getChildByName<Button *>("Button_Confirm");
    btn_confirm->addClickEventListener(
                                       [=](Ref* sender)
                                       {
                                           const std::string& rankName = UserData::getInstance()->getRankName();
                                           if (rankName.compare("") == 0)  return;
                                           
//                                           UserData::getInstance()->Update_Value(UserDataEnum::UserName, Value(rankName));
                                           Network::getInstance()->SetUserName(rankName);
                                           layout_intputName->setVisible(false);
                                       });
    
}


