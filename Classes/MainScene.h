#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "Common.h"

class MainScene : public cocos2d::Layer
{
    
private:
    
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
//    virtual void onEnter();
//    
//    virtual void onExit();
//    
    virtual void OnEnterTransitionDidFinish();
    
    CREATE_FUNC(MainScene);

};

#endif // __MAIN_SCENE_H__
