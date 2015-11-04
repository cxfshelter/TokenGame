//
//  DiamondBuyScene.cpp
//  TokenGame
//
//  Created by best on 15/6/24.
//
//

#include "DiamondBuyScene.h"
#include "DiamondBuyUI.h"

Scene* DiamondBuyScene::createScene()
{
    auto scene = Scene::create();
    
    auto layer = DiamondBuyScene::create();
    
    scene->addChild(layer);
    
    return scene;
}

bool DiamondBuyScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    DiamondBuyUI::getInstance()->Load(this,"DiamondBuyUI.csb");
    DiamondBuyUI::getInstance()->Show();
    
    return true;
}