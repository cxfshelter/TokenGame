
#include "TalentUI.h"
#include "TalentScene.h"

Scene* TalentScene::createScene()
{
    auto scene = Scene::create();

    auto layer = TalentScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool TalentScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    TalentUI::getInstance()->Load(this,"TalentUI.csb");
    TalentUI::getInstance()->Show();
    
    
    return true;
}


