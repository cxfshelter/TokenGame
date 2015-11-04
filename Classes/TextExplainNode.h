//
//  TextExplainNode.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__TextExplainNode__
#define __TokenGame__TextExplainNode__

#include "UIBase.h"
#include "Common.h"

//消息列表
class TextExplainNode
{
private:
    
    static TextExplainNode* sm_TextExplainNode;
    
public:
    
    static TextExplainNode* getInstance();
    
    bool Init();
    
    // cfgId: 兵种id，若为0则不显示兵种相克
    void UpdateText(Node* rootWidget, const string* text, int cfgId = 0);
    

};


#endif /* defined(__TokenGame__TextExplainNode__) */
