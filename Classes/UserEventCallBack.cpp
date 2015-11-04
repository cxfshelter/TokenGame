//
//  UserEventCallBack.cpp
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#include "MakeBuildUI.h"
#include "AchievementSystem.h"

#include "UserEventCallBack.h"

void UserEventCallBack::RegisterCallBack()
{
    AichemyEvent();
    UnLockEvent();
    CollectEvent();
    UpdateUserDataEvent();

}

//void UserEventCallBack::LoadFinishUI()
//{
//    
//}

//更新数据事件
void UserEventCallBack::UpdateUserDataEvent()
{
    UserEventEntity eventEntity;
    eventEntity.m_UserEventName = "SaveUserData";
    eventEntity.m_UserEventType = UserEventType::Aichemy;
    
    //0用户基本数据 1地图数据
    eventEntity.m_CallBack = [](const int id, const Value& value)
    {
        if(id == 0) ;//用户基本数据或地图数据
        UserData::getInstance()->SaveUserData();
    };
    UserEventContainer::getInstance()->Register(eventEntity);
}

//练金事件
void UserEventCallBack::AichemyEvent()
{
    
    UserEventEntity eventEntity;
    eventEntity.m_UserEventType = UserEventType::Aichemy;
    
    eventEntity.m_CallBack = [](const int id, const Value& value)
    {
        CCLOGINFO("Aichemy id=%d",id);
        
        AchievementSystem::getInstance()->CheckAchievement(UserEventType::Aichemy, 1);
    };
    
    UserEventContainer::getInstance()->Register(eventEntity);

    if (UserData::getInstance()->Get_MVI_Value(UserDataEnum::MakeBuild, (int)CfgIdEnum::StorehouseId) == Value::Null)
    {
        UserEventEntity eventEntity;
        eventEntity.m_UserEventName = "Aichemy_StorehouseId";
        eventEntity.m_UserEventType = UserEventType::Aichemy;
        
        eventEntity.m_CallBack = [](const int id, const Value& value)
        {
            UserData::getInstance()->Update_VMI_Fun(UserDataEnum::MakeBuild, (int)CfgIdEnum::StorehouseId, 0);
            UserEventContainer::getInstance()->UnRegister(UserEventType::Aichemy, "Aichemy_StorehouseId");
            MsgListNode::getInstance()->NewMsg("你采集到一颗晶石，机器人建议你建造一座补给站来存放它们");
        };
        UserEventContainer::getInstance()->Register(eventEntity);
    }
}

//采集事件
void UserEventCallBack::CollectEvent()
{
    UserEventEntity eventEntity;
    eventEntity.m_UserEventType = UserEventType::CollectEvent;
    
    eventEntity.m_CallBack = [](const int id, const Value& value)
    {
        CCLOGINFO("CollectEvent id=%d",id);
        
        AchievementSystem::getInstance()->CheckAchievement(UserEventType::CollectEvent, 1);
    };
    UserEventContainer::getInstance()->Register(eventEntity);
    
    
    //判断是否有民宅
    if (!MakeBuildUI::getInstance()->IsMakeWorkHome())
    {
        UserEventEntity eventEntity;
        eventEntity.m_UserEventName = "frist_Collect";
        eventEntity.m_UserEventType = UserEventType::CollectEvent;
        
        eventEntity.m_CallBack = [](const int id, const Value& value)
        {
            auto vm_MakeBuild = UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild);
            
            UserData::getInstance()->Update_VMI_Fun(&vm_MakeBuild->asValueMap(), (int) CfgIdEnum::ShopId, 0);
            UserData::getInstance()->Update_VMI_Fun(&vm_MakeBuild->asValueMap(), (int) CfgIdEnum::FarmId, 0);
            UserData::getInstance()->Update_VMI_Fun(&vm_MakeBuild->asValueMap(), (int) CfgIdEnum::WorkerHouseMinId, 0);
            
            MsgListNode::getInstance()->NewMsg("有了原石和晶炭后，你可以开始建造居住站和培养站了");
            
            UserEventContainer::getInstance()->UnRegister(UserEventType::CollectEvent, "frist_Collect");
            
            // 增加新手手势
            UIUtils::getInstance()->syncLeadHandShowUI("ProductionUI.csb", true);
        };
        UserEventContainer::getInstance()->Register(eventEntity);
    }
}

//解锁相关事件
void UserEventCallBack::UnLockEvent()
{
    UserEventEntity eventEntity;
    eventEntity.m_UserEventType = UserEventType::MakeBuildEvent;
    eventEntity.m_CallBack = [](const int id, const Value& value)
    {
        CCLOGINFO("MakeBuildEvent id=%d",id);
        UnLockSystem::getInstance()->CheckUnLockEvent(UserEventType::MakeBuildEvent, id);
    };
    UserEventContainer::getInstance()->Register(eventEntity);
    
    eventEntity.m_UserEventType = UserEventType::GetItemEvent;
    eventEntity.m_CallBack = [](const int id, const Value& value)
    {
        CCLOGINFO("GetItemEvent id=%d",id);
        UnLockSystem::getInstance()->CheckUnLockEvent(UserEventType::GetItemEvent, id);
    };
    UserEventContainer::getInstance()->Register(eventEntity);
    
    eventEntity.m_UserEventType = UserEventType::CaptureBuildEvent;
    eventEntity.m_UserEventName = "UnLockSystem"; //增加兵工厂建筑Id
    eventEntity.m_CallBack = [](const int id, const Value& value)
    {
        CCLOGINFO("CaptureBuildEvent id=%d",id);
        UnLockSystem::getInstance()->CheckUnLockEvent(UserEventType::CaptureBuildEvent, id);
    };
    UserEventContainer::getInstance()->Register(eventEntity);
    
    eventEntity.m_UserEventType = UserEventType::GoToNewMapEvent;
    eventEntity.m_CallBack = [](const int id, const Value& value)
    {
        CCLOGINFO("GoToNewMapEvent id=%d",id);
        UnLockSystem::getInstance()->CheckUnLockEvent(UserEventType::GoToNewMapEvent, id);
    };
    UserEventContainer::getInstance()->Register(eventEntity);
    
}
