
#include "TemplateScene.h"


Scene* TemplateScene::createScene()
{
    auto scene = Scene::create();

    auto layer = TemplateScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool TemplateScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
//
//    MainUI::getInstance()->Load(this,"MainUI.csb");
//    MainUI::getInstance()->Show();
    
    return true;
}



