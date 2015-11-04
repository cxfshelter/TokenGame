//
//  RankScene.cpp
//  TokenGame
//
//  Created by best on 15/5/11.
//
//

#include "RankScene.h"
#include "RankUI.h"

Scene * RankScene::CreateScene()
{
    auto scene = Scene::create();
    
    auto layer = RankScene::create();
    
    scene->addChild(layer);
    
    return scene;
}

bool RankScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    RankUI::getInstance()->Load(this, "RankListUI.csb");
    RankUI::getInstance()->Show();
    
    return true;
}