//
//  ProgressBar.cpp
//  TokenGame
//
//  Created by token on 15/4/5.
//
//

#include "ProgressBar.h"

ProgressBar::~ProgressBar()
{
    m_CallBack = nullptr;
    m_LoadingBar = nullptr;
}

//暂停
void ProgressBar::Pause()
{
    m_State = false;
}

//恢复
void ProgressBar::Resume()
{
    m_State = true;
}

//恢复原状 重新开始
void ProgressBar::Restart()
{
    Resume();
    m_RemainTime = m_TotalTime;
}

//更新时间
void ProgressBar::Update(float time)
{
    if (m_State)
    {
        float percent = 0;
        m_RemainTime -= time;
        if (m_RemainTime <= 0)
        {
            if (m_LoadingBar != nullptr)
            {
                m_LoadingBar->setPercent(percent);
            }

             if(m_CallBack != nullptr) m_CallBack();
//            bool isAuto = UserData::getInstance()->GetUserData(UserDataEnum::AutoAttack)->asInt() == 1;
//            if(m_CallBack != nullptr && isAuto)
//                m_CallBack();
        }else
        {
            percent = (m_RemainTime / m_TotalTime) * 100;
            if (m_LoadingBar != nullptr)
            {
                m_LoadingBar->setPercent(percent);
            }
        }
    }
}

//获取总时间
float ProgressBar::GetTotalTime()
{
    return m_TotalTime;
}

//获取剩余时间
float ProgressBar::GetRemainTime()
{
    return m_RemainTime;
}

//增加剩余时间
void ProgressBar::AddRemainTime(float time)
{
    m_RemainTime += time;
    if (m_RemainTime > m_TotalTime)
    {
        m_RemainTime = m_TotalTime;
    }
    m_LoadingBar->setPercent((m_RemainTime / m_TotalTime) * 100);
}


//获取剩余时间
void ProgressBar::SetTotalTime(float totalTime, bool isKong)
{
    Resume();
    m_TotalTime = totalTime;
    if (isKong)     m_RemainTime = 0;
    else            m_RemainTime = totalTime;
    
}

//设置剩余时间
void ProgressBar::SetRemainTime(float remainTime)
{
    m_RemainTime = remainTime;
    if (m_RemainTime > m_TotalTime)
    {
        m_RemainTime = m_TotalTime;
    }
    if(m_LoadingBar != nullptr)
    m_LoadingBar->setPercent((m_RemainTime / m_TotalTime) * 100);
}



string ProgressBar::GetScheduleName()
{
    return m_ScheduleName;
}

void ProgressBar::SetScheduleName(string name)
{
    m_ScheduleName = name;
}

//获得进度条
LoadingBar* ProgressBar::GetLoadingBar()
{
    return m_LoadingBar;
}

void ProgressBar::SetLoadingBar(LoadingBar* loadingBar)
{
    m_LoadingBar = loadingBar;
}

//进度为0时回调
void ProgressBar::SetCallBack(std::function<void()> callBack)
{
    m_CallBack = callBack;
}

void ProgressBar::TrigCallBack()
{
    if (m_RemainTime <= 0)
    {
        if (m_LoadingBar != nullptr)
        {
            m_LoadingBar->setPercent(0);
        }
        
        if(m_CallBack != nullptr) m_CallBack();
    }
}


