//
//  MakeBuildUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__MakeBuildUI__
#define __TokenGame__MakeBuildUI__

#include "UIBase.h"
#include "Common.h"

//建造
class MakeBuildUI : public UIBase
{

private:
    bool Init();
    
    static MakeBuildUI* sm_MakeBuildUI;
    //MakeBuildUI(const MakeBuildUI& other){};
    MakeBuildUI& operator = (const MakeBuildUI& other){return *this;};
    
protected:
    
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
    //判断指定建筑是否民宅
    bool IsWorkHome(int buildId);
    
public:
    
    static MakeBuildUI* getInstance();
    
    void ReleaseInstance();
    
    //判断是否有那房
    bool IsMakeWorkHome();
    
    void ButtonEvent(int cfgId);
    
    enum class MakeBuildStatus
    {
        //有新建筑可建
        NewBuild,
        //有建筑可建
        MakeBuild,
        //建造完成
        MakeFinish,
    };
    
};

#endif /* defined(__TokenGame__MakeBuildUI__) */
