

#include "AchievementUI.h"
#include "AchievementScene.h"


Scene* AchievementScene::createScene()
{
    auto scene = Scene::create();

    auto layer = AchievementScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool AchievementScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    AchievementUI::getInstance()->Load(this,"AchievementUI.csb");
    AchievementUI::getInstance()->Show();
    
    return true;
}



