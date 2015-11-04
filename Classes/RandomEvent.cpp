//
//  RandomEvent.cpp
//  TokenGame
//
//  Created by token on 15/4/22.
//
//

#include "RandomEvent.h"
#include "RandomEngine.h"

RandomEvent* RandomEvent::sm_RandomEvent = nullptr;

RandomEvent* RandomEvent::getInstance()
{
    if (sm_RandomEvent == nullptr)
    {
        sm_RandomEvent = new (std::nothrow) RandomEvent();
        if (sm_RandomEvent->Init()) {}
    }
    return sm_RandomEvent;
}

bool RandomEvent::Init()
{
    return true;
}



//随机事件 itemId:物品Id,randomItemNum:随机的数量,randomGetItemNum:获得的数量 都大于0才能获取
bool RandomEvent::GetRandomEvent(int& eventId, int& itemId, int& randomItemNum, int& randomGetItemNum)
{
    
    bool result = true; //反回true 获取随机事件
    
    auto tableIdList =  CfgData::getInstance()->GetCfgTableId("RandomRes");
    
    auto randomMaxNum = (int)tableIdList->size();
    
    auto randomVal = RandomEngine::getInstance()->GetDefaultRandomValue(0, randomMaxNum -1);
    
    auto randomEventId = tableIdList->at(randomVal);

    CfgDataRow dataRow(randomEventId); eventId = randomEventId;
    
    itemId = dataRow.GetValue(CfgField::ItemID)->asInt();
    
    auto vm_Storehouse = &UserData::getInstance()->GetUserData(UserDataEnum::Storehouse)->asValueMap();
    
    auto eventType = dataRow.GetValue(CfgField::EventType)->asInt();
    auto iter_Storehouse = vm_Storehouse->find(StringUtils::format("%d", itemId));
    if (iter_Storehouse == vm_Storehouse->end() && eventType != 2)
    {
        result = false;
    }else
    {
        auto itemNum = 0;
        if (eventType != 2) {
            itemNum = iter_Storehouse->second.asInt();
        }
        if (itemNum < 10 && eventType != 2)
        {
            result = false;
        }else
        {
            auto randomMinNum = dataRow.GetValue(CfgField::RanNumMin)->asInt();
            auto randomMaxNum = dataRow.GetValue(CfgField::RanNumMax)->asInt();
            auto randomNum = RandomEngine::getInstance()->GetDefaultRandomValue(randomMinNum, randomMaxNum);
            
            //随机事件值
            if(eventType == 1) {
                randomItemNum = itemNum * randomNum / 100;
            }
            else
            {
                randomItemNum = randomNum;
            }
            
            auto totalWeight = 0;
            vector<pair<int, int>> vec_pair;
            auto vmi_Reward = dataRow.GetValue(CfgField::Reward_M)->asIntMultiKeyMap();
            for (auto keyVal : vmi_Reward)
            {
                totalWeight +=  keyVal.second.asInt();
                vec_pair.push_back(pair<int,int>(keyVal.first, keyVal.second.asInt()));
            }
            auto randomSeed = RandomEngine::getInstance()->GetDefaultRandomSeed();
            
            //随机获取的值
            randomGetItemNum = RandomEngine::getInstance()->GetRandomWeights(vec_pair, totalWeight, randomSeed);
            randomGetItemNum = randomItemNum * randomGetItemNum;
        }
    }
    return result;
}

//获得随机天赋 反回 false 没随机到
bool RandomEvent::GetRandomTalent(int& eventId, int& talentId)
{
    
    auto result = false;
    
    auto tableIdList =  CfgData::getInstance()->GetCfgTableId("RandomTalent");
    auto randomMaxNum = (int)tableIdList->size();
    
    auto randomVal = RandomEngine::getInstance()->GetDefaultRandomValue(0, randomMaxNum-1);
    
    eventId = tableIdList->at(randomVal);
    
    CfgDataRow dataRow(eventId);
    
    talentId = dataRow.GetValue(CfgField::TalentId)->asInt();
    
    auto mapId = dataRow.GetValue(CfgField::MapId)->asInt();
    auto triggerTalent = dataRow.GetValue(CfgField::TriggerTalentId)->asInt();
    
    
    auto vm_ExploreMaps = &UserData::getInstance()->GetUserData(UserDataEnum::ExploreMaps)->asValueMap();
    auto vv_TalentList = &UserData::getInstance()->GetUserData(UserDataEnum::TalentList)->asValueVector();
    
    if (mapId == 0 && triggerTalent == 0)
    {
        if(find(vv_TalentList->begin(), vv_TalentList->end(), Value(talentId)) == vv_TalentList->end())
        {
            result = true;
        }
        
    }else if(triggerTalent != 0 && mapId == 0)
    {
        if(find(vv_TalentList->begin(), vv_TalentList->end(), Value(triggerTalent)) != vv_TalentList->end())
        {
            if(find(vv_TalentList->begin(), vv_TalentList->end(), Value(talentId)) == vv_TalentList->end())
            {
                result = true;
            }
        }
    }else if(triggerTalent == 0 && mapId != 0)
    {
        if (vm_ExploreMaps->find(StringUtils::format("%d", mapId)) != vm_ExploreMaps->end())
        {
            if(find(vv_TalentList->begin(), vv_TalentList->end(), Value(talentId)) == vv_TalentList->end())
            {
                result = true;
            }
        }
        
    }else
    {
        if(find(vv_TalentList->begin(), vv_TalentList->end(), Value(triggerTalent)) != vv_TalentList->end()
           &&   vm_ExploreMaps->find(StringUtils::format("%d", mapId)) != vm_ExploreMaps->end())
        {
            if(find(vv_TalentList->begin(), vv_TalentList->end(), Value(talentId)) == vv_TalentList->end())
            {
                result = true;
            }
        }
        
    }
    return result;

}


