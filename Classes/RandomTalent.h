//
//  RandomTalent.h
//  TokenGame
//
//  Created by token on 15/3/31.
//
//

#ifndef __TokenGame__RandomTalent__
#define __TokenGame__RandomTalent__

#include "Common.h"

class RandomTalent
{
    
private:
    bool Init();
    
    static RandomTalent* sm_RandomTalent;
    
    vector<int>* m_TalentList;

public:
    
    static RandomTalent* getInstance();
    
    //随机出一个天赋
    int GetRandomTalent();
    
    //购买一个天赋 talentId   itemId 金币 或 宝石
    bool BuyTalent(int talentId, int itemId);
    
};

#endif /* defined(__TokenGame__RandomTalent__) */
