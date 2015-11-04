//
//  UnLockSystem.cpp
//  TokenGame
//
//  Created by token on 15/3/27.
//
//


#include "CfgData.h"
#include "UserData.h"
#include "EnunClass.h"
#include "UnLockSystem.h"
#include "UserEventEnum.h"


UnLockSystem* UnLockSystem::sm_UnLockSystem = nullptr;

//vector<vector< tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey> > >* UnLockSystem::sm_UnLockData = nullptr;

UnLockSystem* UnLockSystem::getInstance()
{
    if (sm_UnLockSystem == nullptr)
    {
        sm_UnLockSystem = new (std::nothrow) UnLockSystem();
        if(sm_UnLockSystem->Init() == false)
        {
            
        }
        
    }
    return sm_UnLockSystem;
}

void UnLockSystem::LoadUnLockData()
{
    int32_t unlockEventId;
    
    const ValueMapIntKey* cfgRowData;
    
    //      事件Id     触发条件       获取结果
    tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey> tupleData;
    
    const vector<uint32_t>* vi = CfgData::getInstance()->GetCfgTableId("UnlockEvent");
    ValueVector* vv = &UserData::getInstance()->GetUserData(UserDataEnum::UnLockIdList)->asValueVector();
    
    vector<tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey>>* eventData;
    vector<tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey>>* getItemEvent = &sm_UnLockData->at((int)UserEventType::GetItemEvent);
    vector<tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey>>* makeBuildEvent = &sm_UnLockData->at((int)UserEventType::MakeBuildEvent);
    vector<tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey>>* goToNewMapEvent = &sm_UnLockData->at((int)UserEventType::GoToNewMapEvent);
    vector<tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey>>* captureBuildEvent = &sm_UnLockData->at((int)UserEventType::CaptureBuildEvent);
    
    
    for (auto vi_iter = vi->begin(); vi_iter != vi->end(); vi_iter++)
    {
        unlockEventId = *vi_iter;
        if(find(vv->begin(), vv->end(), Value(unlockEventId)) == vv->end())
        {
            eventData = nullptr;
            switch (unlockEventId / 1000)
            {
                case 300001:
                    eventData = makeBuildEvent;
                    break;
                case 300002:
                    eventData = getItemEvent;
                    break;
                case 300003:
                    eventData = captureBuildEvent;
                    break;
                case 300004:
                    eventData = goToNewMapEvent;
                    break;
                    
                default:
                    break;
            }
            
            if (eventData != nullptr)
            {
                cfgRowData = CfgData::getInstance()->GetCfgRow(unlockEventId);
                if (cfgRowData == nullptr)  continue;
                tupleData = make_tuple(unlockEventId, cfgRowData->at((int)CfgField::TriggerID_M).asIntMultiKeyMap(),
                                       cfgRowData->at((int)CfgField::FunctionID_M).asIntMultiKeyMap());
                
                eventData->push_back(tupleData);
                CfgData::getInstance()->DeleteCfgId(unlockEventId);
            }
            
        }
    }

}

bool UnLockSystem::Init()
{
    sm_UnLockData = new (std::nothrow) vector<vector< tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey> > >(4,
                    vector< tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey>>());
    return true;
}

// 清空相关变量
void UnLockSystem::releaseData()
{
    if (sm_UnLockData != nullptr) {
        sm_UnLockData->clear();
        sm_UnLockData = nullptr;
    }
    
    delete sm_UnLockSystem;
    sm_UnLockSystem = nullptr;
}


bool UnLockSystem::CheckUnLockEvent(UserEventType userEventType, int32_t triggerId, int num)
{
    if ((int)userEventType > 4) return false;

    vector<tuple<uint32_t, ValueMapIntMultiKey, ValueMapIntMultiKey>>* eventData = &sm_UnLockData->at((int)userEventType);
    for (auto iter = eventData->begin(); iter != eventData->end(); iter++)
    {
        
        auto eventId = get<0>(*iter);        
        auto vmi_Trigger = &get<1>(*iter);
        auto vmi_Function = &get<2>(*iter);
        
        for (auto vmi_iter= vmi_Trigger->begin(); vmi_iter != vmi_Trigger->end(); vmi_iter++)
        {
            //数量目前没用
            if (vmi_iter->first == triggerId && vmi_iter->second.asInt() <= num)
            {
                if (vmi_Trigger->size() == 1)
                {
                    UnLockData(vmi_Function);
                    Value* vv = UserData::getInstance()->GetUserData(UserDataEnum::UnLockIdList);
                    UserData::getInstance()->Add_VI_Fun(&vv->asValueVector(), eventId);
                    
                    UserData::getInstance()->Add_VI_Fun(UserDataEnum::UnLockTriggerList, triggerId);
                    
                }else
                {
                    bool isExist = true;
                    for (auto vmi_iter = vmi_Trigger->begin(); vmi_iter != vmi_Trigger->end(); vmi_iter++)
                    {
                        if (vmi_iter->first == triggerId)
                        {
                            UserData::getInstance()->Add_VI_Fun(UserDataEnum::UnLockTriggerList, vmi_iter->first);
                            continue;
                        }
                        
                        isExist = UserData::getInstance()->Check_VI_Fun(UserDataEnum::UnLockTriggerList, Value(vmi_iter->first));
                        
                        if (isExist == false) continue;
                        
                    }
                    if (isExist)
                    {
                        UnLockData(vmi_Function);
                        Value* vv = UserData::getInstance()->GetUserData(UserDataEnum::UnLockIdList);
                        UserData::getInstance()->Add_VI_Fun(&vv->asValueVector(), eventId);
                        
                        UserData::getInstance()->Add_VI_Fun(UserDataEnum::UnLockTriggerList, triggerId);
                        
                    }
                    
                }
            }
        }
    }
    
    return false;
}

//是否已有触发条件Id 和 数量
bool UnLockSystem::IsExistTriggerId(int triggerid, int num, int cfgId)
{
    
    
//    100001XXX建造建筑
//    100000XXX获得资源类物品
//    100002XXX获得包升级类物品、队伍升级类物品、练金升级类物品、采集升级类物品
//    100005XXX获得图纸类物品
//    900001XXX野外占领建筑
//    100600XXX到达地图XXX
//    仓库显示的物品包括：
//    100000XXX资源类物品
//    100003XXX开锁类物品
//    100004XXX装备
//    100110XXX任务物品
    
    bool result = false;
//    int head = triggerid / 1000;
//    switch (head)
//    {
//        case 100000:
//        case 100003:
//        case 100004:
//        case 100110:
//        {
//            auto data = UserData::getInstance()->Get_MVI_Value(UserDataEnum::Storehouse, cfgId);
//            if (data != nullptr)
//            {
//                result = data->asInt() >= num ? true : false;
//            }
//            break;
//        }
//        case 100001:
//            result = UserData::getInstance()->Get_MVI_Value(UserDataEnum::MakeBuild, cfgId) == nullptr ? false : true;
//            break;
//        case 100002:
//            result = UserData::getInstance()->Get_MVI_Value(UserDataEnum::MakeItem, cfgId) == nullptr ? false : true;
//            break;
//        case 100600:
//            result = UserData::getInstance()->Get_MVI_Value(UserDataEnum::ExploreMaps, cfgId) == nullptr ? false : true;
//            break;
//        
//        default:
//            CCLOGERROR("IsExistTriggerId error triggerId:%d", triggerid);
//            break;
//    }
//    
    return result;
}

//把达成条件的结果保存到相应用户数据中
void UnLockSystem::UnLockData(ValueMapIntMultiKey* vmi)
{
    ValueMap* vm;
    for (auto iter = vmi->begin(); iter != vmi->end(); iter++)
    {
        switch (iter->first)
        {
            case 100700001: //解锁建造建筑
            {
                vm = &UserData::getInstance()->GetUserData(UserDataEnum::MakeBuild)->asValueMap();
                vm->insert(pair<string, Value>(StringUtils::format("%d", iter->second.asInt()), Value(0)));
                break;
            }
            case 100700002: //解锁制作物品
            {
                vm = &UserData::getInstance()->GetUserData(UserDataEnum::MakeItem)->asValueMap();
                vm->insert(pair<string, Value>(StringUtils::format("%d", iter->second.asInt()), Value(true)));
                break;
            }
            case 100700003: //解锁商店物品
            {
                vm = &UserData::getInstance()->GetUserData(UserDataEnum::ShopItem)->asValueMap();
                vm->insert(pair<string, Value>(StringUtils::format("%d", iter->second.asInt()), Value(true)));
                break;
            }
            case 100700006: //解锁生产
            {
                vm = &UserData::getInstance()->GetUserData(UserDataEnum::Production)->asValueMap();
                vm->insert(pair<string, Value>(StringUtils::format("%d", iter->second.asInt()), Value(0)));
                break;
            }
            default:
                CCLOGERROR("UnLock Type Error typeId:%d", iter->first);
                break;
        }
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    
}


