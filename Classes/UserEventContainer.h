//
//  UserEvent.h
//  TokenGame
//
//  Created by token on 15/3/16.
//
//

#ifndef __USEREVENTCONTAINER_H_
#define __USEREVENTCONTAINER_H_

#include <stdio.h>

#include <vector>
#include <unordered_map>

#include "cocos2d.h"
#include "UserEventEnum.h"
#include "UserEventEntity.h"


USING_NS_CC;
using namespace std;


class UserEventContainer
{
    
private:
    static UserEventContainer* sm_UserEventContainer;
    //存放注册的用户事件
    vector< vector< vector<UserEventEntity> > >* sm_container;
    //存放 监听所有用户事件的回调方法
    vector<std::function<void(const UserEventType serEventType,const int, const Value&)>>* sm_CallBackVector;
    
public:
    
    static UserEventContainer* getInstance();
    
    bool Init();
    
    bool Register(const UserEventEntity userEvent);
    
    void UnRegister(const UserEventType userEventType, const string&  userEventName);
    
    void Send(const UserEventType userEventType, const int iEventData = 0, const Value& vEventData = Value::Null);
    
    void RegisterAllUserEventCallBack(std::function<void(const UserEventType serEventType, const int iEventData, const Value& vEventData)> callBack);

    void releaseData();
    
};




#endif /* defined(__USEREVENTCONTAINER_H_) */
