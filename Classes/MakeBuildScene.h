#ifndef __MakeBuild_Scene_H__
#define __MakeBuild_Scene_H__

#include "Common.h"

class MakeBuildScene : public cocos2d::Layer
{
    
private:
    
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    CREATE_FUNC(MakeBuildScene);

};

#endif // __MakeBuild_Scene_H__
