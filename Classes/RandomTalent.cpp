//
//  RandomTalent.cpp
//  TokenGame
//
//  Created by token on 15/3/31.
//
//

#include "RandomTalent.h"
#include "RandomEngine.h"
#include "AchievementSystem.h"

RandomTalent* RandomTalent::sm_RandomTalent = nullptr;

RandomTalent* RandomTalent::getInstance()
{
    if (sm_RandomTalent == nullptr)
    {
        sm_RandomTalent = new (std::nothrow) RandomTalent();
        if(sm_RandomTalent->Init() == false)
        {
            
        }
        
    }
    return sm_RandomTalent;
}

bool RandomTalent::Init()
{
    int cfgId;
    m_TalentList = new (std::nothrow) vector<int>();
    auto tableIdList = CfgData::getInstance()->GetCfgTableId("RandomTalent");
    auto  userTalentList = &UserData::getInstance()->GetUserData(UserDataEnum::TalentList)->asValueVector();
    for (auto iter = tableIdList->begin(); iter != tableIdList->end(); iter++)
    {
        cfgId = *iter;
        if(find(userTalentList->begin(), userTalentList->end(), Value(cfgId)) == userTalentList->end())
        {
            m_TalentList->push_back(cfgId);
        }
    }
    return true;
}
//获取天赋Id 如果反回 0 没有获取
int RandomTalent::GetRandomTalent()
{
    if (m_TalentList->size() == 0)
    {
        return 0;
    }
    int talentId = RandomEngine::getInstance()->GetDefaultRandomValue(0, (int)m_TalentList->size()- 1);
    
    auto talent_Condition = &CfgData::getInstance()->GetValue(talentId, CfgField::TriggerID_M)->asIntMultiKeyMap();
    if (talent_Condition->empty())
    {
        return talentId;
    }else
    {
        int cfgHead;
        bool result = true;
        for (auto iter = talent_Condition->begin(); iter != talent_Condition->end(); iter++)
        {
            if (result == false) break;
            cfgHead = iter->first / 1000;
            switch (cfgHead)
            {
                case 100500:
                {
                    auto talent_data = &UserData::getInstance()->GetUserData(UserDataEnum::TalentList)->asValueVector();
                    if (find(talent_data->begin(), talent_data->end(), Value(iter->first)) == talent_data->end())
                    {
                        result = false;
                    }
                }
                    break;
                case 100600:
                {
                    auto map_data = &UserData::getInstance()->GetUserData(UserDataEnum::ExploreMaps)->asValueMap();
                    
                    if (map_data->find(StringUtils::format("%d", iter->first)) == map_data->end())
                    {
                        result = false;
                    }
                }
                    break;
                default: result = false;
                    break;
            }
        }
        return result ? talentId : 0;
    }
}

//购买一个天赋 talentId   itemId 金币 或 宝石
bool RandomTalent::BuyTalent(int talentId, int itemId)
{
    
    CfgDataRow dataRow(talentId);
    auto imkm = &dataRow.GetValue(CfgField::PriceID_M)->asIntMultiKeyMap();

    int priceNum = 0;
    auto iter = imkm->find(itemId);
    if (iter == imkm->end())
    {
        return false;
    }else
    {
        priceNum = iter->second.asInt();
    }
    ValueMapIntMultiKey vmi;
    vector<pair<int, int> > lackData;
    vmi.insert(pair<int, Value>(itemId, Value(priceNum)));
    bool delResutl = UserData::getInstance()->DeleteStorehouseData(&vmi, &lackData);
    if (delResutl)
    {
        UserData::getInstance()->Add_VI_Fun(UserDataEnum::TalentList, talentId);
        AchievementSystem::getInstance()->CheckAchievement(UserEventType::StudyTalent, talentId);
        return true;
    }else
    {
        return false;
    }
    
}
