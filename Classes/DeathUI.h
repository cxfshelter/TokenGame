//
//  DeathUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__DeathUI__
#define __TokenGame__DeathUI__

#include "UIBase.h"
#include "Common.h"

class DeathUI : public UIBase
{

private:
    bool Init();
    static DeathUI* sm_DeathUI;
    //DeathUI(const DeathUI& other){};
    DeathUI& operator = (const DeathUI& other){return *this;};
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
private:
    
    void ShowOptionDialog(int heroId);
    
public:
    
    static DeathUI* getInstance();
    
};

#endif /* defined(__TokenGame__DeathUI__) */
