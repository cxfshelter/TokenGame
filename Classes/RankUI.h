//
//  RankUI.h
//  TokenGame
//
//  Created by best on 15/5/11.
//
//

#ifndef __TokenGame__RankUI__
#define __TokenGame__RankUI__

#include "UIBase.h"
#include "Common.h"

class RankUI : public UIBase, TextFieldDelegate
{
public:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
    static RankUI * getInstance();
    
private:
    bool Init();
    static RankUI* sm_RankUI;
    RankUI& operator = (const RankUI& other){ return *this; };
    
    // 竞技场 / 探索度 按钮点击处理事件
    void listShowSwitch();
    
    // 检测是否存在名字（UserDataEnum::UserName）
    void checkName();
    
    // 显示输入名字对话框
    void inputName();
    
private:
    // 是否竞技场排行，刚进去界面是探索度
    bool m_isArena = false;
    
};

#endif /* defined(__TokenGame__RankUI__) */
