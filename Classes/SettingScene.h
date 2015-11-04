//
//  SettingScene.h
//  TokenGame
//
//  Created by best on 15/5/22.
//
//

#ifndef __TokenGame__SettingScene__
#define __TokenGame__SettingScene__

#include "Common.h"

class SettingScene : public cocos2d::Layer
{
    
private:
    
public:
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    CREATE_FUNC(SettingScene);
    
};

#endif /* defined(__TokenGame__SettingScene__) */
