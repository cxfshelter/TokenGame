
#include "MapUI.h"
#include "MapScene.h"
#include "TiledMapUI.h"

Scene* MapScene::createScene()
{
    auto scene = Scene::create();

    auto layer = MapScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool MapScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    MapUI::getInstance()->Load(this,"MapUI.csb");
    MapUI::getInstance()->Show();
    
    return true;
}



