
#include "ProductionUI.h"
#include "ProductionScene.h"


Scene* ProductionScene::createScene()
{
    auto scene = Scene::create();

    auto layer = ProductionScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool ProductionScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    ProductionUI::getInstance()->Load(this,"ProductionUI.csb");
    ProductionUI::getInstance()->Show();
    
    return true;
}



