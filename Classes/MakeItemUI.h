//
//  MakeItemUI.h
//  TokenGame
//
//  Created by token on 15/3/20.
//
//

#ifndef __TokenGame__MakeItemUI__
#define __TokenGame__MakeItemUI__

#include "UIBase.h"
#include "Common.h"

//制作
class MakeItemUI : public UIBase
{

private:
    bool Init();
    static MakeItemUI* sm_MakeItemUI;
    //MakeItemUI(const MakeItemUI& other){};
    MakeItemUI& operator = (const MakeItemUI& other){return *this;};
    
    void makeItemEvent(int cfgId);
    
protected:
    virtual void UnLoad();
    virtual void UpdateUI();
    virtual void LoadFinish();
    
public:
    
    static MakeItemUI* getInstance();
    
};

#endif /* defined(__TokenGame__MakeItemUI__) */
