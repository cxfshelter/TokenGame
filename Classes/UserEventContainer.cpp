//
//  UserEvent.cpp
//  TokenGame
//
//  Created by token on 15/3/16.
//
//

#include "UserEventEntity.h"
#include "UserEventContainer.h"

UserEventContainer* UserEventContainer::sm_UserEventContainer = nullptr;

UserEventContainer* UserEventContainer::getInstance()
{
    if (sm_UserEventContainer == nullptr)
    {
        sm_UserEventContainer = new (std::nothrow) UserEventContainer();
        if(sm_UserEventContainer->Init() == false)
        {
            
        }
        
    }
    return sm_UserEventContainer;
}

bool UserEventContainer::Init()
{
    
    uint16_t userEventTypeMaxVal = (uint16_t)UserEventType::EnumClassMaxVal;
    uint16_t userEventPriorityVal = (uint16_t)UserEventPriority::EnumClassMaxVal;
    //存放注册的用户事件
    sm_container = new (nothrow) vector< vector< vector< UserEventEntity > > >(
     userEventTypeMaxVal, vector< vector < UserEventEntity > >(userEventPriorityVal));
    
    //存放监听所有用户事件的回调方法
    sm_CallBackVector = new (std::nothrow) vector<std::function<void(
         const UserEventType serEventType, const int, const Value&)>>();
    
    
    return  true;
}

void UserEventContainer::releaseData()
{
    if (sm_container != nullptr) {
        sm_container->clear();
        sm_container = nullptr;
    }
    if (sm_CallBackVector != nullptr) {
        sm_CallBackVector->clear();
        sm_CallBackVector = nullptr;
    }
    delete sm_UserEventContainer;
    sm_UserEventContainer = nullptr;
}

bool UserEventContainer::Register(const UserEventEntity userEvent)
{
    if(userEvent.m_CallBack == NULL) return false;
    
    auto container = &sm_container->at((uint16_t)userEvent.m_UserEventType);
    auto userEventList = &container->at((uint8_t)userEvent.m_UserEventPriority);
    
    auto iter = userEventList->begin();
    while (iter != userEventList->end())
    {
        if( iter->m_UserEventName.compare(userEvent.m_UserEventName) == 0)
        {
            iter->m_IsUnRegister = false;
            iter->m_CallBack = userEvent.m_CallBack;
            iter->m_UserEventType = userEvent.m_UserEventType;
            iter->m_UserEventName = userEvent.m_UserEventName;
            iter->m_UserEventPriority = userEvent.m_UserEventPriority;
            break;
        }
        iter++;
    }
    if (iter == userEventList->end())
    {
        userEventList->push_back(userEvent);
    }
    return  true;
}



void UserEventContainer::UnRegister(const UserEventType userEventType, const string&  userEventName)
{

    auto vecEventPriority = &sm_container->at((uint16_t)userEventType);
    
    for (auto i = 0; i < (int)UserEventPriority::EnumClassMaxVal; i++)
    {
        
        auto eventPriority =  &vecEventPriority->at(i);
        auto iter = eventPriority->begin();
        while (iter != eventPriority->end())
        {
            if(iter->m_UserEventName.compare( userEventName ) != 0)
            {
                iter ++;
            }else
            {
                iter->m_IsUnRegister = true; break;
            }
        }
    }
}

void UserEventContainer::Send(const UserEventType userEventType, const int iEventData, const Value& vEventData )
{
    CCLOGINFO("UserEventType:%d iEventData:%d",(uint16_t)userEventType, iEventData);
    
    auto container = &sm_container->at((uint16_t)userEventType);
    for (auto i = (int)UserEventPriority::EnumClassMaxVal -1; i > 0; i--)
    {
        for(auto entity : container->at(i))
        {
            if (!entity.m_IsUnRegister)
            {
                entity.m_CallBack(iEventData, vEventData);
            }
            for(int cbidx = 0; cbidx < sm_CallBackVector->size(); cbidx++)
            {
                sm_CallBackVector->at(cbidx)(userEventType,iEventData,vEventData);
            }
        }
    }
}

//注册 监听所有用户事件的回调方法
void UserEventContainer::RegisterAllUserEventCallBack(std::function<void(const UserEventType serEventType,const int, const Value&)> callBack)
{
    sm_CallBackVector->push_back(callBack);
}


