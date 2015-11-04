//
//  RandomEvent.h
//  TokenGame
//
//  Created by token on 15/4/22.
//
//

#ifndef __TokenGame__RandomEvent__
#define __TokenGame__RandomEvent__


#include "Common.h"

class RandomEvent
{
    
private:
    bool Init();
    static RandomEvent* sm_RandomEvent;
    RandomEvent& operator = (const RandomEvent& other){return *this;};
    
private:

    
public:
    static RandomEvent* getInstance();
    
    //获得随机天赋 反回 false 没随机到
    bool GetRandomTalent(int& eventId, int& talentId);

    //随机事件 itemId:物品Id,randomItemNum:随机的数量,randomGetItemNum:获得的数量 都大于0才能获取
    bool GetRandomEvent(int& eventId, int& itemId, int& randomItemNum, int& randomGetItemNum);
    
};




#endif /* defined(__TokenGame__RandomEvent__) */
