//
//  UpdateBuffer.h
//  TokenGame
//
//  Created by token on 15/4/9.
//
//

#ifndef __TokenGame__UpdateBuffer__
#define __TokenGame__UpdateBuffer__

#include "Common.h"

class BufferSchedule
{
    
private:
    
    //当前回调间隔剩余时间
    float m_CbIntervalSurplusTime;

    //更新文件剩余时间
    float m_UpdateTextSurplusTime;
    
    //更新文本间隔时间
    float m_UpdateTextIntervalTime = 0.1;
    
    
public:
    
    //持续时间
    float m_TotalTime;
    
    //回调间隔时间
    float m_CbIntervalTime;
    
    //调整器名
    string m_ScheduleName;
    
    //显示buffer的Layout
    Layout* m_LayoutBuffer;
    
    //时间文本
    Text* m_TimeText = nullptr;
    
    
    //是否删除buffer图标 true 删除
    //buffer结束后自动删除或隐藏图标
    bool m_DelOrHidelLayoutBuffer = false;
    
    //进度为0时回调
    std::function<void()> m_CallBack = nullptr;

    
    void Update(float time);
    
    
};

#endif /* defined(__TokenGame__UpdateBuffer__) */
