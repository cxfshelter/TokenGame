//
//  DiamondBuyScene.h
//  TokenGame
//
//  Created by best on 15/6/24.
//
//

#ifndef __TokenGame__DiamondBuyScene__
#define __TokenGame__DiamondBuyScene__

#include "Common.h"

class DiamondBuyScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    CREATE_FUNC(DiamondBuyScene);
    
private:
    
    
    
};


#endif /* defined(__TokenGame__DiamondBuyScene__) */
