//
//  UserEvent.h
//  TokenGame
//
//  Created by token on 15/3/16.
//
//

#ifndef __USEREVENTENTITY_H_
#define __USEREVENTENTITY_H_

#include "cocos2d.h"
#include "UserEventEnum.h"

USING_NS_CC;


class UserEventEntity
{
public:
    
    bool m_IsUnRegister = false;
    
    std::string m_UserEventName;
    
    UserEventType m_UserEventType;
    
    UserEventPriority m_UserEventPriority = UserEventPriority::Normal;
    
    std::function<void(const int iEventData, const Value& vEventData)> m_CallBack;

    
};



#endif /* defined(__USEREVENTENTITY_H_) */
