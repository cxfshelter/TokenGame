

#include "UserData.h"
#include "SettingData.h"
#include "LoadScene.h"
#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "SoundUtils.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("星际之门");
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    auto res320_480 = Size(320,480);
    auto res640_960 = Size(640,960);
    auto designResolutionSize = Size(640,960);
    auto frameSize = glview->getFrameSize();
    
    float scaleValue;
    if (frameSize.height >= res640_960.height)
    {
//        scaleValue = MIN(frameSize.height/designResolutionSize.height, frameSize.width/designResolutionSize.width);
        scaleValue = MIN(res640_960.height/designResolutionSize.height, res640_960.width/designResolutionSize.width);
        director->setContentScaleFactor(scaleValue);
    }
//    else
//    {
//        scaleValue = MIN(res320_480.height/designResolutionSize.height, res320_480.width/designResolutionSize.width);
//        director->setContentScaleFactor(scaleValue);
//    }
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
#else
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_WIDTH);
#endif
    
    //用于多分辨率
    //FileUtils::getInstance()->setSearchResolutionsOrder(paths);
    
    std::vector<std::string> paths;
    paths.push_back("tk");
    paths.push_back("res");
    paths.push_back("Shaders");
    
    paths.push_back("Language");
    
    paths.push_back("TiledMap");
    paths.push_back("res/UI");
    paths.push_back("res/fonts");
    paths.push_back("res/Button");
    paths.push_back("res/Sound");
    
    paths.push_back("res/animation/anMonsterAll");
    
    FileUtils::getInstance()->setSearchResolutionsOrder(paths);
    
    //paths.clear();
    
    paths.push_back("/Volumes/best/SourceCode/TokenGame/Resources/");
    paths.push_back("/Volumes/best/SourceCode/TokenGame/Resources/res");

    paths.push_back(FileUtils::getInstance()->getWritablePath()+"/AppData");
    
    FileUtils::getInstance()->setSearchPaths(paths);
    
    // create a scene. it's an autorelease object
    auto scene = LoadScene::createScene();

    // run
    director->runWithScene(scene);
    
    //http://www.cnblogs.com/linux-ios/archive/2013/04/13/3019037.html
    SoundUtils::getInstance()->playMusicSound(MUSIC_FILE, true, true);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    UserData::getInstance()->SaveUserData();
    
    // if you use SimpleAudioEngine, it must be pause
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    UserData::getInstance()->m_SleepStatus = true;
    
    //UserData::getInstance()->LoadUserData();
    
    // if you use SimpleAudioEngine, it must resume here
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
