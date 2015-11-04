
#include "TraingHeroUI.h"
#include "TraingHeroScene.h"


Scene* TraingHeroScene::createScene()
{
    auto scene = Scene::create();

    auto layer = TraingHeroScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool TraingHeroScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    TraingHeroUI::getInstance()->Load(this,"TraingHeroUI.csb");
    TraingHeroUI::getInstance()->Show();
    
    return true;
}



