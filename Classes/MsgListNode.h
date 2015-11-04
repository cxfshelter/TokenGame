//
//  MsgListNode.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__MsgListNode__
#define __TokenGame__MsgListNode__

#include "Common.h"


//消息列表
class MsgListNode
{
private:
    bool Init();
    
    //显示信息
    void ShowMsg();
    
    Node* m_RootNode;
    
    vector<string>* m_MsgList;
    
    static MsgListNode* sm_MsgListNode;
    
    int m_index = 0;
    
    // 是否正在执行自动下滑特效
    bool isRunningAction = false;
    
    Text * lastAddText = nullptr;
    
public:
    
    static MsgListNode* getInstance();
    
    void UnLoadMsg();
    void LoadMsg(Node* rootWidget);
    
    void NewMsg(string msg);
    
    void NewMsg(LanguageIdEnum langId);
    
    void NewMsg(int cfgId , CfgField field);
    
    void NewMsg(int cfgId , int itemNum, CfgField field);
    
    // 剧情文字
    void addPlotMsg(string msg, Node * nodeSchedule);
    
    // 添加至列表，参数：内容、是否播动画
    void addMsg(string msg, bool isRunAction);
    
    // 播放列表内items的移动, isPlay：false - stop items' action, true - start items' action
    void playItemsMoveAction(int rows, bool isPlay);
    
    // 重启清空
    bool clearMsg();
    
};


#endif /* defined(__TokenGame__MsgListNode__) */
