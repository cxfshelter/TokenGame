#ifndef __BattlePub_Scene_H__
#define __BattlePub_Scene_H__

#include "Common.h"

class BattlePubScene : public cocos2d::Layer
{
    
private:
    
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    CREATE_FUNC(BattlePubScene);

};

#endif // __BattlePub_Scene_H__
