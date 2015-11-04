//
//  AchievementSystem.cpp
//  TokenGame
//
//  Created by token on 15/3/31.
//
//

#include "AchievementSystem.h"

AchievementSystem* AchievementSystem::sm_AchievementSystem = nullptr;

AchievementSystem* AchievementSystem::getInstance()
{
    if (sm_AchievementSystem == nullptr)
    {
        sm_AchievementSystem = new (std::nothrow) AchievementSystem();
        if (sm_AchievementSystem->Init()) {}
    }
    return sm_AchievementSystem;
}

bool AchievementSystem::Init()
{
    LoadData();
    return true;
}

// 清空相关变量
void AchievementSystem::releaseData()
{
    if (m_AchievementMapData != nullptr) {
        m_AchievementMapData->clear();
        m_AchievementMapData = nullptr;
    }
    
    delete sm_AchievementSystem;
    sm_AchievementSystem = nullptr;
}

void AchievementSystem::LoadData()
{
    
    if (m_AchievementMapData != nullptr) return;
    
    //    成就类型对应的            成就Id  条件
    m_AchievementMapData = new (std::nothrow) unordered_map<int, vector<pair<int, int>>>((int) AchievementType::EnumClassMaxVal);
    
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::Aichemy, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::AthleticsLayer, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::BattleEatBread, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::BuryHero, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::CollectEvent, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::DefeatMonster, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::ExploreMap, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::FinishMap, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::GambleVictory, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::KillMonster, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::StudyTalent, vector<pair<int, int>>()));
    m_AchievementMapData->insert(pair<int, vector<pair<int, int>>>((int)UserEventType::TrainHero, vector<pair<int, int>>()));
    
    
    auto  tableIdList = CfgData::getInstance()->GetCfgTableId("Achievement");
    auto  finish = &UserData::getInstance()->GetUserData(UserDataEnum::FinishAchievement)->asValueVector();

    //  成就Id  条件
    pair<int, int> pair_data;
    for (auto iter = tableIdList->begin(); iter != tableIdList->end(); iter++)
    {
        auto cfgId = *iter;
        if (find(finish->begin(), finish->end(), Value((int)cfgId))!= finish->end())
        {
            continue;
        }
        
        CfgDataRow dataRow(cfgId);
        pair_data = make_pair(cfgId, dataRow.GetValue(CfgField::Parameter)->asInt());
        switch (cfgId / 1000)
        {
            case (int)AchievementType::Aichemy:
                m_AchievementMapData->at((int) UserEventType::Aichemy).push_back(pair_data);
                break;
            case (int)AchievementType::AthleticsLayer :
                
                m_AchievementMapData->at((int) UserEventType::AthleticsLayer).push_back(pair_data);
                
                break;
            case (int)AchievementType::BattleEatBread :
                m_AchievementMapData->at((int) UserEventType::BattleEatBread).push_back(pair_data);
                
                break;
            case (int)AchievementType::BuryHero:
                
                m_AchievementMapData->at((int) UserEventType::BuryHero).push_back(pair_data);
                break;
            case (int)AchievementType::CollectEvent:
                m_AchievementMapData->at((int) UserEventType::CollectEvent).push_back(pair_data);
                
                break;
            case (int)AchievementType::DefeatMonster:
                m_AchievementMapData->at((int) UserEventType::DefeatMonster).push_back(pair_data);
                
                break;
            case (int)AchievementType::ExploreMap:
                m_AchievementMapData->at((int) UserEventType::ExploreMap).push_back(pair_data);
                
                break;
            case (int)AchievementType::FinishMap:
                m_AchievementMapData->at((int) UserEventType::FinishMap).push_back(pair_data);
                break;
                
            case (int)AchievementType::GambleVictory:
                m_AchievementMapData->at((int) UserEventType::GambleVictory).push_back(pair_data);
                break;
                
            case (int)AchievementType::KillMonster:
                m_AchievementMapData->at((int) UserEventType::KillMonster).push_back(pair_data);
                break;
                
            case (int)AchievementType::StudyTalent:
                m_AchievementMapData->at((int) UserEventType::StudyTalent).push_back(pair_data);
                break;
                
            case (int)AchievementType::TrainHero:
                m_AchievementMapData->at((int) UserEventType::TrainHero).push_back(pair_data);
                break;
                
            default:
                break;
        }
    }
    
}

 
//检查是否与成就相关操作
void AchievementSystem::CheckAchievement(UserEventType userEventType, int IdOrNum)
{
    auto iter_Data = m_AchievementMapData->find((int)userEventType);
    if (iter_Data == m_AchievementMapData->end()) return;
    
    switch (userEventType)
    {
        case UserEventType::Aichemy:
            CountType(UserEventType::Aichemy, IdOrNum);
            break;
        case UserEventType::AthleticsLayer :
            CountType(UserEventType::AthleticsLayer, IdOrNum);
            break;
        case UserEventType::KillMonster:
            CountType(UserEventType::KillMonster, IdOrNum);
            break;
        case UserEventType::BattleEatBread :
            CountType(UserEventType::BattleEatBread, IdOrNum);
            break;
        case UserEventType::CollectEvent:
            CountType(UserEventType::CollectEvent, IdOrNum);
            break;
        case UserEventType::ExploreMap:
            CountType(UserEventType::ExploreMap, IdOrNum);
            break;
        case UserEventType::GambleVictory:
            CountType(UserEventType::GambleVictory, IdOrNum);
            break;
            
        case UserEventType::DefeatMonster:
            MatchingType(UserEventType::DefeatMonster, IdOrNum);
            break;
        case UserEventType::BuryHero:
            MatchingType(UserEventType::BuryHero, IdOrNum);
            break;
        case UserEventType::FinishMap:
            MatchingType(UserEventType::FinishMap, IdOrNum);
            break;
        case UserEventType::StudyTalent:
            MatchingType(UserEventType::StudyTalent, IdOrNum);
            break;
        case UserEventType::TrainHero:
            MatchingType(UserEventType::TrainHero, IdOrNum);
            break;
        default:
            break;
    }
    
}
void AchievementSystem::MatchingType(UserEventType userEventType,int IdOrNum)
{
    auto data = &m_AchievementMapData->at((int)userEventType);
    for (auto iter = data->begin(); iter != data->end(); iter++)
    {
        if (iter->second == IdOrNum)
        {
            AddAchievementNum(iter->first);
            data->erase(iter);
            break;
        }
    }
}

//增加成就值
void AchievementSystem::AddAchievementNum(int cfgId)
{
    // 弹出达成文案
    UIUtils::getInstance()->showAchievementMoveText(cfgId);
    
    UserData::getInstance()->Add_VI_Fun(UserDataEnum::FinishAchievement, cfgId);
    
    auto nowNum  = UserData::getInstance()->GetUserData(UserDataEnum::AchievementNum)->asInt();
    CfgDataRow dataRow(cfgId);
    auto addNum = dataRow.GetValue(CfgField::Reward)->asInt();
    auto totalNum = nowNum + addNum;
    
    int talentId = 0, cfgVal = 0;
    auto tableIdList = CfgData::getInstance()->GetCfgTableId("AchievementTalent");
    for (auto iter = tableIdList->begin(); iter != tableIdList->end(); iter ++)
    {
        cfgId = *iter;
        CfgDataRow dataRow(cfgId);
        cfgVal = dataRow.GetValue(CfgField::Parameter)->asInt();
        
        if (totalNum < cfgVal) continue;
        
        if (totalNum == cfgVal || (totalNum - addNum) < cfgVal)
        {
            talentId = dataRow.GetValue(CfgField::TalentID)->asInt();
            UserData::getInstance()->Add_VI_Fun(UserDataEnum::TalentList, talentId);
            break;
        }
    }
    
    UserData::getInstance()->Update_Value(UserDataEnum::AchievementNum, Value(totalNum));
}

//计算类型的成就
void AchievementSystem::CountType(UserEventType userEventType, int IdOrNum)
{
    int finishData = 0;
    int cfgCondition = 0;
    int achievementId = 0;
    
    auto unFinish = &UserData::getInstance()->GetUserData(UserDataEnum::UnFinishAchievement)->asValueMap();
    auto data = &m_AchievementMapData->at((int)userEventType);
    for (auto iter = data->begin(); iter != data->end(); iter++)
    {
        achievementId = iter->first;
        cfgCondition = iter->second;
        auto key = StringUtils::format("%d", achievementId);
        auto iter_ufa = unFinish->find( key );
        if (iter_ufa != unFinish->end())
        {
            finishData = iter_ufa->second.asInt() + IdOrNum;
            iter_ufa->second = Value(finishData);
            if (finishData >= cfgCondition)
            {
                // 这里勋章进度 finishData 超过任务要求则没有进行记录（勋章进度只从未完成处查找）
                AddAchievementNum(achievementId);
                iter = data->erase(iter);
                unFinish->erase(iter_ufa);
                break;
            }
        }
		else
        {
            unFinish->insert(pair<string, Value>( key, Value(IdOrNum)));
        }
    }
//    UserData::getInstance()->SaveUserData();
    UserData::getInstance()->SaveCorUserData(false);
    
}

int AchievementSystem::getCurProgressNum(int cfgId)
{
    int progressNum = 0;
    
    auto unFinish = &UserData::getInstance()->GetUserData(UserDataEnum::UnFinishAchievement)->asValueMap();
    auto key = StringUtils::format("%d", cfgId);
    auto iter_ufa = unFinish->find( key );
    if (iter_ufa != unFinish->end()) {
        progressNum = iter_ufa->second.asInt();
    }
    
    return progressNum;
}
