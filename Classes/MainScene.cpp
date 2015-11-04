

#include "MainUI.h"
#include "MainScene.h"


Scene* MainScene::createScene()
{
    auto scene = Scene::create();

    auto layer = MainScene::create();

    scene->addChild(layer);

    return scene;
}

// on "init" you need to initialize your instance
bool MainScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    MainUI::getInstance()->Load(this,"MainUI.csb");
    
    MainUI::getInstance()->Show();
    
    return true;
}
//
//void MainScene::onEnter()
//{
//    
//}
//
//void MainScene::onExit()
//{
//    
//}
//
void MainScene::OnEnterTransitionDidFinish()
{
        
}



