//
//  MainUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__MainUI__
#define __TokenGame__MainUI__

#include "UIBase.h"
#include "Common.h"
#include "ProgressBar.h"

//主界面
class MainUI : public UIBase
{

private:
    bool Init();
    static MainUI* sm_MainUI;
    
    MainUI& operator = (const MainUI& other){return *this;};
    
private:
    int m_opacity = 255;
    int m_opacity2 = 0;

    ProgressBar* m_ProgressBar = nullptr;
    
    //随机事件时间间隔 秒
    // best test
    //    const float m_RandomEventTimeInterval = 5;
    const float m_RandomEventTimeInterval = 600;
    
    //随机事件名
    const string m_RandomEventName = "RandomEventName";
    
    //炼金 调度器名
    const string m_AlchemyProgressBar = "AlchemyProgressBar";
    
    //炼金回调
    void AlchemyCallBackEvent();
    
    void CheckRandomEvent();
    
    //炼金获取的物品
    void AlchemyGetItem();
    
    //加载 mainUi 时检查随机时间是否已到
    void CheckRandomEventTime();
    
    //增加 随机事件调度器
    void AddRandomEventSchedule(int intervalTime);
    
    //随机天赋对话框
    void RandomTalentDialog(int eventId, int talentId);

    //随机事件对话框
    void RandomEventDialog(int eventId, int itemId, int randomItemNum, int randomGetItemNum);
    
    // 首次进入则产生剧情
    void PlayFirstTimePlot();
    
    // 黑幕渐变
    void SlowClear();
    
    /*  服务器返回日期 - 首次登陆日期 + 1 = 可领取的对应天数
     返回可领取的对应天数
     
     若：首次登陆时间 == 0，则取 服务器返回日期，当做首日登陆，再调用自身
     */
    void checkTimeWithServer();
    
    // 显示登陆奖励
    // param1: 距离首次登陆的天数（1则代表今天是首次登陆）
    void showLoginAward(int loginDay);

protected:
    
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static MainUI* getInstance();
    
    void ReleaseInstance();
    
    /*   检测是否需要左右滑，
     */
    void checkSlideEvent();

    void ShowButton();
    void SetButtonEvent();
    void UpdateStorehouse();
    
};

#endif /* defined(__TokenGame__MainUI__) */
