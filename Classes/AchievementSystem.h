//
//  AchievementSystem.h
//  TokenGame
//
//  Created by token on 15/3/31.
//
//

#ifndef __TokenGame__AchievementSystem__
#define __TokenGame__AchievementSystem__

#include "Common.h"

class AchievementSystem
{
private:
    
    bool Init();
    static AchievementSystem* sm_AchievementSystem;
    AchievementSystem& operator = (const AchievementSystem& other){return *this;};
    
private:
    
    
    void AddAchievementNum(int cfgId);
    
    void CountType(UserEventType userEventType,int IdOrNum);
    
    void MatchingType(UserEventType userEventType,int IdOrNum);
    
    
private:
    //    成就类型对应的            成就Id  条件
    unordered_map<int, vector<pair<int, int>>>* m_AchievementMapData;
    
public:
    
    static AchievementSystem* getInstance();
    
    // 清空相关变量
    void releaseData();
    
    void LoadData();
    void CheckAchievement(UserEventType userEventType, int IdOrNum);
    
    // 得到对应的进度数，成就对应的成就cfgId
    int getCurProgressNum(int cfgId);
    
};

#endif /* defined(__TokenGame__AchievementSystem__) */
