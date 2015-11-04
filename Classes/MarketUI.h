//
//  MarketUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__MarketUI__
#define __TokenGame__MarketUI__

#include "UIBase.h"
#include "Common.h"

class MarketUI : public UIBase
{

private:
    bool Init();
    static MarketUI* sm_MarketUI;
    MarketUI& operator = (const MarketUI& other){return *this;};

private:
    
    uint32_t m_BuildId = 0;
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static MarketUI* getInstance();
    
};

#endif /* defined(__TokenGame__MarketUI__) */
