//
//  RankScene.h
//  TokenGame
//
//  Created by best on 15/5/11.
//
//

#ifndef __TokenGame__RankScene__
#define __TokenGame__RankScene__

#include "Common.h"

class RankScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene * CreateScene();
    
    virtual bool init();
    
    CREATE_FUNC(RankScene);
    
private:
    
};


#endif /* defined(__TokenGame__RankScene__) */
