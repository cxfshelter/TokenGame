//
//  TalentUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__TalentUI__
#define __TokenGame__TalentUI__

#include "UIBase.h"
#include "Common.h"

//天赋
class TalentUI : public UIBase
{

private:
    bool Init();
    static TalentUI* sm_TalentUI;
    TalentUI& operator = (const TalentUI& other){return *this;};
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static TalentUI* getInstance();
    
};

#endif /* defined(__TokenGame__TalentUI__) */
