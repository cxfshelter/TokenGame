//
//  RandomEngine.cpp
//  TokenGame
//
//  Created by token on 15/3/27.
//
//

#include "RandomEngine.h"

using namespace std;

RandomEngine* RandomEngine::sm_RandomEngine = nullptr;

RandomEngine* RandomEngine::getInstance()
{
    if (sm_RandomEngine == nullptr)
    {
        sm_RandomEngine = new (std::nothrow) RandomEngine();
        if(sm_RandomEngine->Init() == false)
        {
            
        }
        
    }
    return sm_RandomEngine;
}

bool RandomEngine::Init()
{
    SetDefaultRandomSeed();
    return true;
}
//获取当前设置的随机种子
uint32_t RandomEngine::GetRandomSeed()
{
    return m_Now_User_Seed;
}

//获取默认的随机种子
uint32_t RandomEngine::GetDefaultRandomSeed()
{
    return m_Default_Seed;
}

//设置当前要使用的随机种子
void RandomEngine::SetRandomSeed(uint32_t seed)
{
    m_Now_User_Seed = seed;
    m_NowSeed_random_engine.seed(seed);
}

//使用默认随机种子
void RandomEngine::SetDefaultRandomSeed(uint32_t seed)
{
    if (seed != 0)
    {
        m_Default_Seed = seed;
    }else
    {
        m_Default_Seed = (uint32_t)time(nullptr);
    }
    m_Default_random_engine.seed(m_Default_Seed);
}

//使用当前设置的随机种子生成随机数
//生成min到max随机数（包含 min max)
uint32_t RandomEngine::GetRandomValue(uint32_t min, uint32_t max)
{
    std::uniform_int_distribution<unsigned> uid(min, max);
    return uid(m_NowSeed_random_engine);
}

//使用默认设置的随机种子生成随机数
//生成min到max随机数（包含 min max)
uint32_t RandomEngine::GetDefaultRandomValue(uint32_t min, uint32_t max)
{
    std::uniform_int_distribution<unsigned> uid(min, max);
    return uid(m_Default_random_engine);
}

/// <summary>
/// 随机权重 反回随机后的Key  totalWights 当前使用随机的种子
/// </summary>
int RandomEngine::GetRandomWeights(vector<pair<int,int>>& vec, int totalWights, uint32_t randomSeed)
{
    int randomVal = 0;
    if( randomSeed == GetDefaultRandomSeed())
    {
        randomVal = GetDefaultRandomValue(1, totalWights);
    }else
    {
        randomVal = GetRandomValue(1, totalWights);
    }
    
    int result = 0;
    for(auto iter = vec.begin(); iter != vec.end(); iter++)
    {
        result = iter->first;
        if (randomVal < iter->second )
        {
            result = iter->first; break;
        }
        else
        {
            randomVal -= iter->second;
        }
    }
    return result;
}
