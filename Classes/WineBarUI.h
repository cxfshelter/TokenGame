//
//  WineBarUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__WineBarUI__
#define __TokenGame__WineBarUI__

#include "UIBase.h"
#include "Common.h"

class WineBarUI : public UIBase
{

private:
    bool Init();
    static WineBarUI* sm_WineBarUI;
    WineBarUI& operator = (const WineBarUI& other){return *this;};
    
private:
    
    uint32_t m_BuildId = 0;
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static WineBarUI* getInstance();
    
};

#endif /* defined(__TokenGame__WineBarUI__) */
