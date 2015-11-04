

#include "MakeBuildUI.h"
#include "MakeBuildScene.h"


Scene* MakeBuildScene::createScene()
{
    auto scene = Scene::create();

    auto layer = MakeBuildScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool MakeBuildScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    MakeBuildUI::getInstance()->Load(this,"MakeBuildUI.csb");
    MakeBuildUI::getInstance()->Show();
    
    return true;
}



