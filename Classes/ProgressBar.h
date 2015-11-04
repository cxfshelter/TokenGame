//
//  ProgressBar.h
//  TokenGame
//
//  Created by token on 15/4/5.
//
//

#ifndef __TokenGame__ProgressBar__
#define __TokenGame__ProgressBar__

#include "Common.h"

class ProgressBar
{
    
private:
    
    //true 正常
    //false 暂停
    bool m_State = false;
    
    //总时间
    float m_TotalTime;
    
    //剩余时间
    float m_RemainTime;
    
    //用于删除 Schedule;
    string m_ScheduleName;
    
    //进度条控件
    LoadingBar* m_LoadingBar;
    
    
    
public:
    
    ~ProgressBar();
    
    //进度为0时回调
    std::function<void()> m_CallBack;
    
    //暂停
    void Pause();
    //恢复
    void Resume();
    
    //复原
    void Restart();
    
    //获取总时间
    float GetTotalTime();
    
    //获取剩余时间
    float GetRemainTime();
    
    //更新时间
    void Update(float time);
    
    string GetScheduleName();
    
    void SetScheduleName(string Name);
    
    
    //增加剩余时间
    void AddRemainTime(float time);
    
    //设置总时间
    void SetTotalTime(float totalTime, bool isKong = false);
    
    //设置剩余时间
    void SetRemainTime(float remainTime);
    
    //获取进度条
    LoadingBar* GetLoadingBar();
    
    //设置进度条
    void SetLoadingBar(LoadingBar* loadingBar);
    
    //进度为0时回调
    void SetCallBack(std::function<void()> callBack);
    
    //触发事件
    void TrigCallBack();
    
};


#endif /* defined(__TokenGame__ProgressBar__) */
