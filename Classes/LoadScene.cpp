
#include "LoadScene.h"
#include "MainScene.h"
#include "MapScene.h"

#include "Common.h"

#include "UserEventCallBack.h"

#include "MainUI.h"

#include "AchievementUI.h"

#include "BattlePubUI.h"

#include "ProductionUI.h"

#include "MakeBuildUI.h"

#include "MakeItemUI.h"

#include "ShopUI.h"

#include "TalentUI.h"

#include "TraingHeroUI.h"

#include "AchievementSystem.h"

#include "Network.h"

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    #include "ExcelToCfgData.h"
//#endif

USING_NS_CC;

using namespace ui;


Scene* LoadScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoadScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoadScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    auto rootNode = CSLoader::createNode("LoadingUI.csb");
    if (rootNode != nullptr)
    {
        auto size = Director::getInstance()->getVisibleSize();
        if(size.height / size.width > 1.65)
        {
            auto backImg1136 = rootNode->getChildByName("BackImg1136");
            auto backImg960 = rootNode->getChildByName("BackImg960");
            backImg1136->setVisible(true);
            backImg960->setVisible(false);
        }
        addChild(rootNode);
    }
    
//    auto visibleSize = Director::getInstance()->getVisibleSize();
//
//    auto sprite1 = Sprite::create("HelloWorld.png");
//    sprite1->setPosition(visibleSize.width/2, visibleSize.height /2 );
//    sprite1->setScale(3);
//    this->addChild(sprite1);
//    
//    auto sprite2 = Sprite::create("fangzi1.png");
//    sprite2->setPosition(visibleSize.width/2 - 100, visibleSize.height * 0.83 );
//    sprite2->setScale(3);
//    auto shader_program = GLProgram::createWithFilenames("shadow.vsh", "shadow.fsh");
//    shader_program->use();
//    shader_program->setUniformsForBuiltins();
//    
//    sprite2->setGLProgram(shader_program);
//    this->addChild(sprite2);
    
    // 增加了相关变量释放，
    // 注释：// best release
    
    LoadCfgData();
    SettingData::getInstance()->LoadSettingData();
    UserData::getInstance()->LoadUserData();
    DiamondBuyData::getInstance()->LoadDiamondData();
    AchievementSystem::getInstance()->LoadData();
    UnLockSystem::getInstance()->LoadUnLockData();
    MultiLanguage::getInstance()->SetLanguageName();
    UserEventCallBack userEvent;
    userEvent.RegisterCallBack();
    UIUtils::getInstance()->plistImgInit();
    
//    UnLockSystem::getInstance()->CheckUnLockEvent(UserEventType::MakeBuildEvent, 100001110);
//    UnLockSystem::getInstance()->CheckUnLockEvent(UserEventType::MakeBuildEvent, 100001113);
//    
//    
    
    //通知消息的node结点
    //Director::getInstance()->getNotificationNode();
    
    //缓存数据 提高运行速度
    //Director::getInstance()->getTextureCache();
    
    //Director::getInstance()->getTextureCache()->addImageAsync(const std::string &filepath, const std::function<void (Texture2D *)> &callback)
    
    this->schedule(schedule_selector(LoadScene::UpdateTime), 1, 100, 1);
    
//    
//    Network::getInstance()->UpdateProductionTime();
//    Network::getInstance()->GetProductionTime([](int time)
//                                              {
//                                                  
//                                              });
//    
//    Network::getInstance()->GetRanking([](vector<tuple<int,int,string>>* explore,
//                                          vector<tuple<int,int,string>>* arena)
//                                       {
//                                           
//                                           
//                                           
//                                       });
    
    
    /*
    MainUI::getInstance()->Load(this,"MainUI.csb");
    
    AchievementUI::getInstance()->Load(this,"AchievementUI.csb");
    
    BattlePubUI::getInstance()->Load(this,"BattlePubUI.csb");
    
    ProductionUI::getInstance()->Load(this,"ProductionUI.csb");
    
    MakeBuildUI::getInstance()->Load(this,"MakeBuildUI.csb");
    
    MakeItemUI::getInstance()->Load(this,"MakeItemUI.csb");
    
    ShopUI::getInstance()->Load(this,"ShopUI.csb");
    
    TalentUI::getInstance()->Load(this,"TalentUI");
    
    TraingHeroUI::getInstance()->Load(this,"TraingHeroUI.csb");
    */
    
//    ValueMap TestVM;
//    TestVM.insert(pair<std::string, Value>("100000",Value("xxxxxx")));
//    TestVM.insert(pair<std::string, Value>("100000",Value("xxxxxx")));
//    
//    FileUtils::getInstance()->writeToFile(TestVM, "/Cocos2d/CocosProject/test.xml");
    return true;
}

void LoadScene::UpdateTime(float dt)
{
    //loadTime += dt;
    //auto label = (Label*)getChildByName("label");
    //label->setString(StringUtils::format("Time:%f",loadTime));
    
//    if (loadTime > 1)
//    {
//        auto scene = MainScene::createScene();
//        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
//    }
    
    Scene* scene;
    bool isInMap = UserData::getInstance()->GetUserData(UserDataEnum::IsPlayerInMap)->asBool();
    if (isInMap) {
        scene = MapScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    }
    else {
        scene = MainScene::createScene();
        Director::getInstance()->replaceScene(CCTransitionMoveInL::create(0.18f, scene));
    }
    
}


//测试代码
void  flip(int i)
{
    int newindex[i];
    int index[] = { 5, 6, -1, 3, 2};
    int* niptr = newindex; int* iptr = index + i;
    for (int j = i; j >= 0; j--)
    {
        *niptr = *iptr;
        niptr++; iptr--;
    }
    niptr = newindex; iptr = index;
    for (int n = 0; n <= i; n++)
    {
        *iptr = *niptr;
        niptr++; iptr++;
    }

}

void LoadScene::LoadCfgData()
{
    //flip(3);
    
    //if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    
    ExcelToCfgData excel;

    excel.m_ExcelFileDir = "/Volumes/best/PlanningDocument/ExcelFile/";
    excel.m_BinaryFileDir = "/Volumes/best/SourceCode/TokenGame/Resources/";
    excel.m_CfgFieldFileDir = "/Volumes/best/SourceCode/TokenGame/Resources/";
    
    // 生成.tk配置文件
    excel.RunExcelToCfgData();
    
    //#endif
    
//    vim
//    每行的行首都添加一个字符串：%s/^/要插入的字符串
//    每行的行尾都添加一个字符串：%s/$/要插入的字符串
    vector<string> tkFiles =
    {
        "Achievement.tk",
        "AchievementTalent.tk",
        "BuffList.tk",
        "BuildingMonster.tk",
        "Buildings.tk",
        "CemeteryList.tk",
        "FieldStoreList.tk",
        "ItemDropList.tk",
        "Items.tk",
        "Map.tk",
        "MissionItems.tk",
        "DiamondItem.tk",
        "RandomRes.tk",
        "RandomTalent.tk",
        "SkillList.tk",
        "Talent.tk",
        "Training.tk",
        "UniversalBuilding.tk",
        "UnlockEvent.tk",
        "AttackCoefficient.tk",
        "VoidMonster.tk",
        "Map1Building.tk",
        "Map1Monster.tk",
        "Map2Building.tk",
        "Map2Monster.tk",
        "Map3Building.tk",
        "Map3Monster.tk",
        "Map4Building.tk",
        "Map4Monster.tk",
        "Map5Building.tk",
        "Map5Monster.tk",
        "Map6Building.tk",
        "Map6Monster.tk",
        "Map7Building.tk",
        "Map7Monster.tk",
        "Map8Building.tk",
        "Map8Monster.tk",
        "Map9Building.tk",
        "Map9Monster.tk",
        "Map10Building.tk",
        "Map10Monster.tk",
        "Map11Building.tk",
        "Map11Monster.tk",
        "Map12Building.tk",
        "Map12Monster.tk",
    };
    for (auto tk : tkFiles)
    {
        CfgData::getInstance()->LoadCfgFile(tk);
    }
}



