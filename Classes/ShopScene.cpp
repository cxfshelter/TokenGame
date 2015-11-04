
#include "ShopUI.h"
#include "ShopScene.h"


Scene* ShopScene::createScene()
{
    auto scene = Scene::create();

    auto layer = ShopScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool ShopScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    ShopUI::getInstance()->Load(this,"ShopUI.csb");
    ShopUI::getInstance()->Show();
    
    return true;
}



