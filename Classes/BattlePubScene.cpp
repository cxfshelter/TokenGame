
#include "BattlePubUI.h"
#include "BattlePubScene.h"


Scene* BattlePubScene::createScene()
{
    auto scene = Scene::create();

    auto layer = BattlePubScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool BattlePubScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    BattlePubUI::getInstance()->Load(this,"BattlePubUI.csb");
    BattlePubUI::getInstance()->Show();
    
    return true;
}



