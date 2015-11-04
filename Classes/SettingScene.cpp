//
//  SettingScene.cpp
//  TokenGame
//
//  Created by best on 15/5/22.
//
//

#include "SettingScene.h"
#include "SettingUI.h"


Scene* SettingScene::createScene()
{
    auto scene = Scene::create();
    
    auto layer = SettingScene::create();
    
    scene->addChild(layer);
    
    return scene;
}

// on "init" you need to initialize your instance
bool SettingScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    SettingUI::getInstance()->Load(this,"SettingUI.csb");
    SettingUI::getInstance()->Show();
    
    return true;
}
