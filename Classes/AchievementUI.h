//
//  AchievementUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__AchievementUI__
#define __TokenGame__AchievementUI__

#include "UIBase.h"

//成就
class AchievementUI : public UIBase
{

private:
    bool Init();
    static AchievementUI* sm_AchievementUI;
    AchievementUI& operator = (const AchievementUI& other){return *this;};
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static AchievementUI* getInstance();
  };

#endif /* defined(__TokenGame__AchievementUI__) */
