//
//  BattleBagUI.h
//  TokenGame
//
//  Created by best on 15/5/6.
//
//

#ifndef __TokenGame__BattleBagUI__
#define __TokenGame__BattleBagUI__

#include "UIBase.h"

class BattleBagUI : public UIBase
{
private:
    bool Init();
    static BattleBagUI* sm_BattleBagUI;
    BattleBagUI& operator = (const BattleBagUI& other){ return *this; };
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    static BattleBagUI* getInstance();
    
    
};

#endif /* defined(__TokenGame__BattleBagUI__) */
