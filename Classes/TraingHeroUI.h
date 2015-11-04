//
//  TraingHeroUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__TraingHeroUI__
#define __TokenGame__TraingHeroUI__

#include "UIBase.h"
#include "Common.h"

//训练 
class TraingHeroUI : public UIBase
{

private:
    bool Init();
    static TraingHeroUI* sm_TraingHeroUI;
    TraingHeroUI& operator = (const TraingHeroUI& other){return *this;};
    
private:
    
    //招募 英雄
    void RecruitDialog();
    
    //升级面板
    void UpgradeUI(int cfgId);
    
    void ConfirmUI(int oldCfgId, int newCfgId);
    
    
protected:
    
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static TraingHeroUI* getInstance();
    
};

#endif /* defined(__TokenGame__TraingHeroUI__) */
