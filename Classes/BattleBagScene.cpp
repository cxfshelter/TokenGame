//
//  BattleBagScene.cpp
//  TokenGame
//
//  Created by best on 15/5/6.
//
//

#include "BattleBagScene.h"
#include "BattleBagUI.h"

Scene* BattleBagScene::createScene()
{
    auto scene = Scene::create();
    
    auto layer = BattleBagScene::create();
    
    scene->addChild(layer);
    
    return scene;
}

bool BattleBagScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    BattleBagUI::getInstance()->Load(this, "BattleBagUI.csb");
    BattleBagUI::getInstance()->Show();
    
    return true;
}