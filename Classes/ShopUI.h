//
//  ShopUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__ShopUI__
#define __TokenGame__ShopUI__

#include "UIBase.h"
#include "Common.h"

//商店
class ShopUI : public UIBase
{

private:
    bool Init();
    static ShopUI* sm_ShopUI;

    ShopUI& operator = (const ShopUI& other){return *this;};
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static ShopUI* getInstance();
    
};

#endif /* defined(__TokenGame__ShopUI__) */
