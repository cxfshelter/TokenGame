//
//  BattleBagScene.h
//  TokenGame
//
//  Created by best on 15/5/6.
//
//

#ifndef __TokenGame__BattleBagScene__
#define __TokenGame__BattleBagScene__

#include "Common.h"

class BattleBagScene : public cocos2d::Layer
{
private:
    
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    CREATE_FUNC(BattleBagScene);
    
};

#endif /* defined(__TokenGame__BattleBagScene__) */
