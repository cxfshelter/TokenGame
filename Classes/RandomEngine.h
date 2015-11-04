//
//  RandomEngine.h
//  TokenGame
//
//  Created by token on 15/3/27.
//
//

#ifndef __TokenGame__RandomEngine__
#define __TokenGame__RandomEngine__

#include <random>

class RandomEngine
{
private:
    
    static RandomEngine* sm_RandomEngine;
    
    //默认的随机种子
    uint32_t m_Default_Seed = 0;
    //当前使用的随机种子
    uint32_t m_Now_User_Seed = 0;
    
    //默认的随机种子引擎
    std::default_random_engine  m_Default_random_engine;
    
    //当前使用的随机种子引擎
    std::default_random_engine  m_NowSeed_random_engine;
    
private:
    bool Init();
    
public:
    
    static RandomEngine* getInstance();
    
    //获取当前设置的随机种子
    uint32_t GetRandomSeed();
    
    //获取默认的随机种子
    uint32_t GetDefaultRandomSeed();
    
    //设置当前要使用的随机种子
    void SetRandomSeed(uint32_t seed);
    
    //使用默认随机种子
    void SetDefaultRandomSeed(uint32_t seed = 0);
    
    //使用当前设置的随机种子生成随机数
    //生成min到max随机数（包含 min max)
    uint32_t GetRandomValue(uint32_t min, uint32_t max);
    
    //使用默认设置的随机种子生成随机数
    //生成min到max随机数（包含 min max)
    uint32_t GetDefaultRandomValue(uint32_t min, uint32_t max);
    
    //随机权重 反回随机后的Key
    int GetRandomWeights(std::vector<std::pair<int,int>>& vec, int totalWights, uint32_t randomSeed);
};

#endif /* defined(__TokenGame__RandomEngine__) */
