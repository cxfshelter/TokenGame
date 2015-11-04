

#include "MakeItemUI.h"
#include "MakeItemScene.h"


Scene* MakeItemScene::createScene()
{
    auto scene = Scene::create();

    auto layer = MakeItemScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool MakeItemScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    MakeItemUI::getInstance()->Load(this,"MakeItemUI.csb");
    MakeItemUI::getInstance()->Show();
    
    return true;
}



