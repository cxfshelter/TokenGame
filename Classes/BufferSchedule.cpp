//
//  UpdateBuffer.cpp
//  TokenGame
//
//  Created by token on 15/4/9.
//
//

#include "BufferSchedule.h"


//更新时间
void BufferSchedule::Update(float time)
{
    
    m_TotalTime -= time;
    m_CbIntervalSurplusTime -= time;
    m_UpdateTextSurplusTime -= time;
    if (m_UpdateTextSurplusTime <= 0)
    {
        m_UpdateTextSurplusTime =  m_UpdateTextIntervalTime;
        if (m_TimeText != nullptr)
        {
            m_TotalTime = m_TotalTime < 0 ? 0 : m_TotalTime;
            m_TimeText->setString(StringUtils::format("%.1f", m_TotalTime));
        }
    }
    
    if (m_CbIntervalSurplusTime <= 0)
    {
        if(m_CallBack != nullptr) m_CallBack();
        m_CbIntervalSurplusTime = m_CbIntervalTime;
    }
    
    if (m_TotalTime <= 0)
    {
        m_LayoutBuffer->unschedule(m_ScheduleName);
        if (m_DelOrHidelLayoutBuffer)
        {
            if (m_LayoutBuffer->getParent())
            {
                m_LayoutBuffer->removeFromParent();
            }
        }else
        {
            m_LayoutBuffer->setVisible(false);
        }
        
        delete this;
    }
}