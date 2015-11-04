//
//  UserEventCallBack.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__UserEventCallBack__
#define __TokenGame__UserEventCallBack__

#include "Common.h"

class UserEventCallBack
{
private:
    
    //练金事件
    void AichemyEvent();
    
    //更新用户数据事件
    void UpdateUserDataEvent();
    
    //采集相关事件
    void CollectEvent();
    
    //解锁相关事件
    void UnLockEvent();

public:
    void RegisterCallBack();
};

#endif /* defined(__TokenGame__UserEventCallBack__) */
