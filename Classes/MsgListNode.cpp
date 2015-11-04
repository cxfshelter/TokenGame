//
//  MsgListNode.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MsgListNode.h"

#define LISTVIEW_MAX_ROWS           20

MsgListNode* MsgListNode::sm_MsgListNode = nullptr;

MsgListNode* MsgListNode::getInstance()
{
    if (sm_MsgListNode == nullptr)
    {
        sm_MsgListNode = new (std::nothrow) MsgListNode();
        if(sm_MsgListNode->Init() == false)
        {
            
        }
        
    }
    return sm_MsgListNode;

}

bool MsgListNode::Init()
{
    m_MsgList = new vector<string>();
    return true;
}

bool MsgListNode::clearMsg()
{
    m_MsgList->clear();
    return true;
}

void MsgListNode::LoadMsg(Node* rootWidget)
{
    m_RootNode = rootWidget;
    ShowMsg();
}

void MsgListNode::UnLoadMsg()
{
    m_RootNode = nullptr;
}

void MsgListNode::NewMsg(string msg)
{
//    auto nowTime = time(nullptr);
//    auto tmPtr = localtime((time_t*)&nowTime);
//    string timeStr = StringUtils::format("%4d-%2d-%2d %2d:%2d:%2d\n", tmPtr->tm_year, tmPtr->tm_mon, tmPtr->tm_mday, tmPtr->tm_hour, tmPtr->tm_min, tmPtr->tm_sec);
    
    if (m_MsgList->size() > 20)
    {
        m_MsgList->pop_back();
    }
    
    // m_MsgList->insert(m_MsgList->begin(), msg);
    m_MsgList->push_back(msg);
    // ShowMsg();
    addMsg(msg, true);
}

void MsgListNode::NewMsg(LanguageIdEnum langId)
{
    NewMsg( MultiLanguage::getInstance()->GetText((int)langId));
}

void MsgListNode::NewMsg(int cfgId , int itemNum, CfgField field)
{
    CfgDataRow dataRow(cfgId);
    auto langId = dataRow.GetValue(field)->asInt();
    auto langText = MultiLanguage::getInstance()->GetText(langId);
    NewMsg(StringUtils::format(langText.c_str(), itemNum));
}

void MsgListNode::NewMsg(int cfgId , CfgField field)
{
    CfgDataRow dataRow(cfgId);
    auto langId = dataRow.GetValue(field)->asInt();
    NewMsg(MultiLanguage::getInstance()->GetText(langId));
}

//显示信息
void MsgListNode::ShowMsg()
{
//    if (m_RootNode == nullptr) return;
//    
//    auto node_MsgList = m_RootNode->getChildByName<Node*>("Node_MsgList");
//    if (node_MsgList == nullptr) return;
//    
//    auto layout_MsgList = node_MsgList->getChildByName<Layout*>("Layout_MsgList");
//    
//    if (layout_MsgList == nullptr)return;
//    
//    auto lview_MsgList = layout_MsgList->getChildByName<ListView*>("LView_MsgList");
//    
//    if (lview_MsgList == nullptr) return;
//    
//    auto text_msg = (Text*)lview_MsgList->getChildByName("Text_Msg");
//    
//    int rows = 0;
//    string textStr;
//    float rowWords = 18;
//    std::u16string utf16String;
//    for (auto str : *m_MsgList)
//    {
//        if (StringUtils::UTF8ToUTF16(str, utf16String))
//        {
//            rows += ceil(utf16String.length() /rowWords );
//        }
//        textStr.append(str + '\n');
//    }
//    text_msg->ignoreContentAdaptWithSize(false);
//    
//    text_msg->setString(textStr);
//    Size textAreaSize(lview_MsgList->getContentSize().width, rows * LIST_ITEM_HEIGHT);
//    text_msg->setTextAreaSize(textAreaSize);
//    lview_MsgList->setInnerContainerSize(textAreaSize);
//    lview_MsgList->scrollToTop(0.5, false);
    
    for(auto str : *m_MsgList)
    {
        addMsg(str, false);
    }
    
}

void MsgListNode::addPlotMsg(string msg, Node * nodeSchedule)
{
    vector<string> splitMsg;
    
    unsigned long startPlace = 0;
    unsigned long endPlace = msg.find('\n');
    string curMsg;
    string nextMsg = msg;
    while (endPlace != -1)
    {
        curMsg = nextMsg.substr(startPlace, endPlace);
        nextMsg = nextMsg.substr(endPlace+sizeof('\n'));
        endPlace = nextMsg.find('\n');
        splitMsg.push_back(curMsg);
    }
    
    m_index = 0;
    nodeSchedule->schedule(
                         [=](float dt)
                         {
                             if (m_index >= splitMsg.size()) {
                                 nodeSchedule->unschedule("PlotPlay");
                                 UserEventContainer::getInstance()->Send(UserEventType::PlotTxtLoadFinish, 0, Value(true));
                             }
                             else
                             {
                                 string str = splitMsg[m_index];
                                 NewMsg(str);
                                 ++m_index;
                             }
                             
                         },
                         3, "PlotPlay");
}

void MsgListNode::addMsg(string msg, bool isRunAction)
{
    if (msg.length() <= 0)
    {
        return;
    }
    
    if (!m_RootNode) {
        return;
    }
    
    auto node_MsgList = m_RootNode->getChildByName<Node*>("Node_MsgList");
    if(!node_MsgList) return;
    
    auto layout_msgList = node_MsgList->getChildByName<Layout *>("Layout_MsgList");
    if(!layout_msgList) return;
    
    auto listview = layout_msgList->getChildByName<ListView *>("LView_MsgList");
    if(!listview) return;
    if (listview->getItems().size() > LISTVIEW_MAX_ROWS) {
        listview->removeLastItem();
    }
    
    // 加入裁剪区域
    ClippingNode * clipNode = layout_msgList->getChildByName<ClippingNode *>("clipNode");
    if (!clipNode) {
        clipNode = ClippingNode::create();
        clipNode->setName("clipNode");
//        clipNode->setContentSize(listview->getContentSize());
        clipNode->setContentSize(Size(listview->getContentSize().width, listview->getContentSize().height));
        clipNode->setAnchorPoint(Vec2(1, 1));
        clipNode->setPosition(listview->getPosition());
        
        //创建裁剪模板，裁剪节点将按照这个模板来裁剪区域
        DrawNode * stencil = DrawNode::create();
        Vec2 recTangle[4];
        recTangle[0] = Vec2(0, 0);
        recTangle[1] = Vec2(clipNode->getContentSize().width, 0);
        recTangle[2] = Vec2(clipNode->getContentSize().width, clipNode->getContentSize().height);
        recTangle[3] = Vec2(0, clipNode->getContentSize().height);
        stencil->drawPolygon(recTangle, 4, Color4F::WHITE, 1, Color4F::WHITE);
        clipNode->setStencil(stencil);
//        clipNode->addChild(stencil);
        
        layout_msgList->addChild(clipNode, 100);
    }
    
    auto lvFirstItem = listview->getItem(0);
    if(!lvFirstItem) return;
    
    auto text_template = node_MsgList->getChildByName<Text *>("Text_Template");
    if(!text_template) return;
    
    auto text = (Text *)text_template->clone();
    text->ignoreContentAdaptWithSize(false);
    text->setString(msg);
    text->setTextHorizontalAlignment(TextHAlignment::LEFT);
    
    int rows = 1;
    std::u16string utf16String;
    if(StringUtils::UTF8ToUTF16(msg, utf16String))
    {
        int remNum = utf16String.length() % LIST_ITEM_HORWORD;
        if (remNum == 0)
        {
            rows = utf16String.length() / LIST_ITEM_HORWORD;
        }
        else
        {
            rows += ceil(utf16String.length() / LIST_ITEM_HORWORD);
        }
    }
    Size textAreaSize(listview->getContentSize().width, rows * text->getContentSize().height);
    text->setTextAreaSize(textAreaSize);

    // 获得lvFirstItem相对于clipNode的坐标（其原来是相对于listView）
    auto pos = clipNode->convertToNodeSpace(lvFirstItem->getWorldPosition());
    // 暂时：当连续快按时，listview[0]的位置取值有误差
    if (pos.y != 230) {
        pos.y = 230;
    }
    
    text->setPosition(Vec2(pos.x, pos.y + rows * LIST_ITEM_HEIGHT));
    clipNode->addChild(text);
    
    if(isRunAction)
    {
        playItemsMoveAction(rows, false);
        lastAddText = text;
        lastAddText->retain();
        lastAddText->runAction(
                       Sequence::create(
                                          MoveTo::create(LIST_MOVE_DURATION, pos),
                                          CallFunc::create(
                                             [=]()
                                             {
                                                 lastAddText->removeFromParent();
                                                 listview->insertCustomItem(lastAddText, 0);
                                                 lastAddText->release();
                                                 lastAddText = nullptr;
                                             }),
                       nullptr
                       ));
        
        playItemsMoveAction(rows, true);
    }
    else
    {
        text->removeFromParent();
        listview->insertCustomItem(text, 0);
    }
}

void MsgListNode::playItemsMoveAction(int rows, bool isPlay)
{
    auto node_MsgList = m_RootNode->getChildByName<Node*>("Node_MsgList");
    auto layout_msgList = node_MsgList->getChildByName<Layout *>("Layout_MsgList");
    auto listview = layout_msgList->getChildByName<ListView *>("LView_MsgList");
    
    if (listview->getItems().empty()) {
        return;
    }
    
    MoveTo * moveAction;
    if (isPlay) {
        for(auto item : listview->getItems())
        {
            auto pos = item->getPosition();
            moveAction = MoveTo::create(LIST_MOVE_DURATION, Vec2(pos.x, pos.y - rows * LIST_ITEM_HEIGHT));
            item->runAction(moveAction);
        }
    }
    else {
        for(auto item : listview->getItems())
        {
            if (item->isRunning()) {
                item->stopAllActions();
            }
        }
        if (lastAddText != nullptr) {
            lastAddText->removeFromParent();
            listview->insertCustomItem(lastAddText, 0);
            lastAddText = nullptr;
        }
    }
}




